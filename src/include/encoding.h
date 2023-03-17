#ifndef SCREEN_ENCODING_H
#define SCREEN_ENCODING_H

#include "window.h"

bool utf8_iscomb (uint32_t);
bool utf8_isdouble (uint32_t);
char *EncodingName (int);
int  FindEncoding (char*);
int CanEncodeFont (int, int);
int ContainsSpecialDeffont (struct mline*, int, int, int);
int DecodeChar (int, int, int*);
int EncodeChar (char*, int, int, int*);
int EncodingDefFont (int);
int FromUtf8 (int, int*);
int LoadFontTranslation (int, char*);
int PrepareEncodedChar (int);
int RecodeBuf (unsigned char*, int, int, int, unsigned char*);
size_t ToUtf8 (char*, uint32_t);
size_t ToUtf8_comb (char*, uint32_t);
struct mchar* recode_mchar (struct mchar*, int, int);
struct mline* recode_mline (struct mline*, int, int, int);
void AddUtf8 (uint32_t);
void InitBuiltinTabs (void);
void LoadFontTranslationsForEncoding (int);
void ResetEncoding (Window* win);
void WinSwitchEncoding (Window* win, int);
void utf8_handle_comb (unsigned int, struct mchar*);

#endif // SCREEN_ENCODING_H
