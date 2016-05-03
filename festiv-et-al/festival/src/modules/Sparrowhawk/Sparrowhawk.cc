/********************************************************************************
 * Sparrowhawk integration code
 *
 * Author: Rob Clark
 *
 * Copyright 2015 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#include <map>
#include "EST.h"
#include "siod.h"
#include "siod_est.h"
#include "EST_Token.h"
#include "festival.h"

#ifdef SUPPORT_SPARROWHAWK

#include "Sparrowhawk.h"

SIOD_REGISTER_CLASS(sparrowhawk_normalizer, Normalizer)
VAL_REGISTER_CLASS(sparrowhawk_normalizer, Normalizer)

// NB. that's a Devanagari danda at the end
static const EST_String sparrowhawk_punc = "()[]{}:;.,\"'?!।";

static std::map<std::string, Normalizer *> normalizers;

static Normalizer *get_normalizer(const EST_String& voice) {
  Normalizer *normalizer = 0;
  auto found = normalizers.find(voice.str());
  if (found !=  normalizers.end()) {
    normalizer = found->second;
  }
  return normalizer;
}

static LISP FT_Sparrowhawk_Init(LISP l_voicename, LISP l_config, LISP l_prefix) {

  const EST_String voice = get_c_string(l_voicename);
  const EST_String config = get_c_string(l_config);
  const EST_String prefix = get_c_string(l_prefix);

  if (!get_normalizer(voice)) {
    Normalizer *n = new Normalizer();
    normalizers[voice.str()] = n;
    n->Setup(config.str(), prefix.str());
 }
  return NIL;
}

static LISP FT_Sparrowhawk_Close(LISP l_voicename) {

  const EST_String voice = get_c_string(l_voicename);
  Normalizer *n = 0;

  auto found = normalizers.find(voice.str());
  if (found != normalizers.end()) {
    n = found->second;
    normalizers.erase(voice.str());
    delete n;
  }
  return NIL;
}

// For processing Sparrowhawk output

static EST_String next_token(EST_TokenStream& ts)
{
  EST_Token t;
  ts >> t;
  if (ts.eof())
    return EST_String::Empty;
  else
    return t.string();
}

static void get_indices(const EST_String& s, int *start, int* end) {
  *start = s.before(",").Int();
  *end = s.after(",").Int();
}


// Creating Festival Token and relations

static void utt_add_word_to_token(EST_Utterance *utt,
                                  const EST_String word_name,
                                  const int token_id) {
  // First find the appropriate token
  EST_Item *token = utt->relation("Token")->head();
  for (; token; token = token->next())
    if (token->features().val("id").Int() == token_id)
      break;
  if (!token) // Probably a sil or similar, so ignore.
    return;

  EST_Item *word = utt->relation("Word")->append();
  word->set_name(word_name);
  append_daughter(token, "Token", word);
}

static LISP FT_Sparrowhawk_Normalize_Utt(LISP l_utt) {

  EST_Utterance *utt = get_c_utt(l_utt);

  // Get a normalizer for the current voice
  LISP l_voice = siod_get_lval("current-voice", "unknown_voice");
  const EST_String voice = get_c_string(l_voice);
  Normalizer *normalizer = get_normalizer(voice);
  if (normalizer == NULL)
    EST_error("No Sparrowhawk normalizer found for voice %s.", (const char *)voice);

  // Run the normalizer
  const EST_String est_input = get_c_string(utt_iform(*utt));
  string output;
  normalizer->NormalizeAndShowLinks(est_input.str(), &output);

  cerr << "Normaliser input: " << est_input.str() << endl;
  cerr << "Normaliser output" << endl << output << endl;

  // Process the normalizer output
  utt->create_relation("Token");
  utt->create_relation("Word");

  EST_TokenStream ts;
  EST_Token tok, t1, t2, t3;

  int token_id, word_id;
  EST_String token_name, word_name;

  int pos_start, pos_end;
  int words_start, words_end;
  bool skip_read_next;

  EST_Item *last_token = 0;
  EST_String pending_prepunc = EST_String::Empty;

  EST_Regex position_pair_re("[0-9]+,[0-9]+");

  ts.open_string(output.c_str());
  ts.set_WhiteSpaceChars("\t\r\n"); // Sparrowhawk allows spaces in Tokens.
  tok = next_token(ts);
  while(tok !=  EST_String::Empty) {
    if (tok == "Token:") {
      token_id = next_token(ts).Int();
      // Some tokens have no name and get skipped!, so get the next 4 tokens and backtrack.
      t1 = next_token(ts);
      t2 = next_token(ts);
      t3 = next_token(ts);
      if (t3 == "Token:" || t3 == "Word:") {
        // Token name was null!
        token_name = "(Semiotic Class)";
        get_indices(t1, &pos_start, &pos_end);
        get_indices(t2, &words_start, &words_end);
        skip_read_next = true;
      } else {
        token_name = t1.String();
        get_indices(t2, &pos_start, &pos_end);
        get_indices(t3, &words_start, &words_end);
        skip_read_next = false;
      }
      if (sparrowhawk_punc.contains(token_name)) { // Punctuation.
        // Punctuation is currently prepunctuation if there is already prepunctuation,
        // or pos_start < pos_end, this last condition is due to a bug in sparrowhark,
        // and will need addressing once fixed.
        if (!last_token || pending_prepunc != EST_String::Empty || pos_start < pos_end) {
          pending_prepunc += token_name;
        } else {
          // cerr << "    setting on previous token." << endl;
         last_token->set("punc", last_token->features().val("punc").String() + token_name);
        }
      } else { // Actual token.
        EST_Item *item = utt->relation("Token")->append();
        item->set_name(token_name);
        item->set("id", token_id);
        item->set("w_start", words_start);
        item->set("w_end", words_end);
        // Prepunctuation.
        if (pending_prepunc != EST_String::Empty) {
          // cerr << "    adding pending prepunc: " << pending_prepunc << endl;
          item->set("prepunctuation", pending_prepunc);
          pending_prepunc = EST_String::Empty;
        } else {
           item->set("prepunctuation", "");
        }
        // Set punctuation null for the moment.
        item->set("punc", "");
        // Update for next iteration.
        last_token = item;
      }
      if (skip_read_next) {
        tok = t3;
      } else {
        tok = next_token(ts);
      }
    } else {
      if(tok == "Word:") {
         word_id = next_token(ts).Int();
         word_name = next_token(ts);
         token_id = next_token(ts).Int();
         utt_add_word_to_token(utt, word_name, token_id);
         tok = next_token(ts);
      } else {
        EST_error("Unknown token: %s", (const char *)(tok.String()));
      }
    }
}
  return l_utt;
}


void festival_Sparrowhawk_init(void)
{
  proclaim_module("Sparrowhawk");

  init_subr_3("sparrowhawk.init", FT_Sparrowhawk_Init,
  "(sparrowhawk.init VOICENAME CONFIG PREFIX)\n\
    Initialise sparrowhawk for the given voice with the given configuration." );

  init_subr_1("sparrowhawk.close", FT_Sparrowhawk_Close,
  "(sparrowhawk.close VOICENAME)\n\
    Clean up sparrowhawk for this voice." );

  festival_def_utt_module("Sparrowhawk_normalize", FT_Sparrowhawk_Normalize_Utt,
                          "(Sparrowhawk_normalize UTT)\n  Text normalization using Sparrowhawk");
}

#else  // Stubs for when not supported

static LISP FT_Sparrowhawk_NULL(LISP l_utt) {
  EST_error("Sparrowhawk not supported in this build!");
  return l_utt;
}

static LISP FT_NULL() {
  EST_error("Sparrowhawk not supported in this build!");
  return NIL;
}

void festival_Sparrowhawk_init(void)
{
  proclaim_module("Sparrowhawk");

  init_subr_0("sparrowhawk.init", FT_NULL,
  "(sparrowhawk.init)\n\
     SPARROWHAWK IS NOT SUPPORTED IN THIS BUILD." );

  init_subr_0("sparrowhawk.close", FT_NULL,
  "(sparrowhawk.close)\n\
    SPARROWHAWK IS NOT SUPPORTED IN THIS BUILD" );

  festival_def_utt_module("Sparrowhawk_normalize", FT_Sparrowhawk_NULL,
                          "(Sparrowhawk_normalize UTT)\n  SPARROWHAWK IS NOT SUPPORTED IN THIS BUILD");

}

#endif // SUPPORT_SPARROWHAWK
