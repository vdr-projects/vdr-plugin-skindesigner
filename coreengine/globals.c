#include "globals.h"
#include "xmlparser.h"
#include "../config.h"
#include <locale.h>

cGlobals::cGlobals(void) {
    customTokenChange = 0;
    lastCustomTokenQuery = 0;

    fonts.insert(pair<string, string>("vdrOsd", Setup.FontOsd));
    fonts.insert(pair<string, string>("vdrFix", Setup.FontFix));
    fonts.insert(pair<string, string>("vdrSml", Setup.FontSml));
    language = Setup.OSDLanguage;
    dsyslog("skindesigner: using language %s", language.c_str());
}

bool cGlobals::ReadFromXML(void) {
    //globals.xml is mandatory
    string xmlFile = "globals.xml";
    cXmlParser parser;
    if (!parser.ReadGlobals(this, xmlFile))
        return false;
    if (!parser.ParseGlobals())
        return false;
    //theme.xml is optional
    xmlFile = "theme.xml";
    if (parser.ReadGlobals(this, xmlFile)) {
        parser.ParseGlobals();
    }
    return true;
}

void cGlobals::AddColor(string &name, tColor &col) {
    colors.erase(name);
    colors.insert(pair<string, tColor>(name, col));
}

bool cGlobals::GetColor(string &name, tColor &col) {
    int size = name.size();
    if (size < 2)
        return false;
    string nameCutted = name.substr(1, size-2);
    map <string, tColor>::iterator hit = colors.find(nameCutted);
    if (hit != colors.end()) {
        col = hit->second;
        return true;
    }
    return false;
}

void cGlobals::AddFont(string &name, string &font) {
    fonts.erase(name);
    fonts.insert(pair<string, string>(name, font));
}

bool cGlobals::GetFont(string name, string &font) {
    int size = name.size();
    if (size < 2)
        return false;
    string nameCutted = name.substr(1, size-2);
    map<string,string>::iterator hit = fonts.find(nameCutted);
    if (hit != fonts.end()) {
        font = hit->second;
        return true;
    }
    return false;    
}

void cGlobals::AddInt(string &name, int value) {
    intVars.erase(name);
    intVars.insert(pair<string, int>(name, value));    
}

void cGlobals::ReplaceIntVars(string &value) {
    for (map<string, int>::iterator it = intVars.begin(); it != intVars.end(); it++) {
        stringstream sToken;
        sToken << "{" << it->first << "}";
        string token = sToken.str();
        size_t foundToken = string::npos;
        do {
            foundToken = value.find(token);
            if (foundToken != string::npos) {
                stringstream st;
                st << it->second;
                value = value.replace(foundToken, token.size(), st.str());
            }
        } while (foundToken != string::npos);
    }
}

bool cGlobals::GetInt(string name, int &val) {
    DeleteCurledBrackets(name);
    map < string, int >::iterator hit = intVars.find(name);
    if (hit != intVars.end()) {
        val = hit->second;
        return true;
    }
    return false;
}

void cGlobals::AddDouble(string &name, string &value) {
    doubleVars.erase(name);
    if (config.replaceDecPoint) {
        if (value.find_first_of('.') != string::npos) {
            std::replace( value.begin(), value.end(), '.', config.decPoint);
        }
    }
    double val = atof(value.c_str());
    doubleVars.insert(pair<string, double>(name, val));    
}

void cGlobals::ReplaceDoubleVars(string &value) {
    for (map<string, double>::iterator it = doubleVars.begin(); it != doubleVars.end(); it++) {
        stringstream sToken;
        sToken << "{" << it->first << "}";
        string token = sToken.str();
        size_t foundToken = string::npos;
        do {
            foundToken = value.find(token);
            if (foundToken != string::npos) {
                stringstream st;
                st << it->second;
                string doubleVal = st.str();
                if (config.replaceDecPoint) {
                    if (doubleVal.find_first_of('.') != string::npos) {
                        std::replace( doubleVal.begin(), doubleVal.end(), '.', config.decPoint);
                    }
                }
                value = value.replace(foundToken, token.size(), doubleVal);
            }
        } while (foundToken != string::npos);
    }    
}

void cGlobals::AddString(string &name, string &value) {
    stringVars.erase(name);
    stringVars.insert(pair<string, string>(name, value));
}

void cGlobals::ReplaceStringVars(string &value) {
    for (map<string,string>::iterator it = stringVars.begin(); it != stringVars.end(); it++) {
        stringstream sToken;
        sToken << "{" << it->first << "}";
        string token = sToken.str();
        size_t foundToken = value.find(token);
        if (foundToken != string::npos) {
            value = value.replace(foundToken, token.size(), it->second);
        }
    }
}

bool cGlobals::GetString(string name, string &value) {
    DeleteCurledBrackets(name);
    map<string,string>::iterator hit = stringVars.find(name);
    if (hit == stringVars.end())
        return false;
    value = hit->second;
    return true;
}

bool cGlobals::AddTranslation(string name,  map < string, string > transl) {
    translations.erase(name);
    translations.insert(pair<string, map < string, string > >(name, transl));
    return true;
}

bool cGlobals::Translate(string text, string &translation) {
    string transStart = "{tr(";
    string transEnd = ")}";
    size_t foundStart = text.find(transStart);
    size_t foundEnd = text.find(transEnd);
    bool translated = false;

    while (foundStart != string::npos && foundEnd != string::npos) {
        string token = text.substr(foundStart + 1, foundEnd - foundStart);
        string transToken = DoTranslate(token);
        if (transToken.size() > 0)
            translated = true;
        else
            return false;
        text.replace(foundStart, foundEnd - foundStart + 2, transToken);
        foundStart = text.find(transStart);
        foundEnd = text.find(transEnd);
    }
    if (translated)
        translation = text;
    return translated;
}

string cGlobals::DoTranslate(string token) {
    string translation = "";
    map <string, map< string, string > >::iterator hit = translations.find(token);
    if (hit == translations.end()) {
        esyslog("skindesigner: invalid translation token %s", token.c_str());
        return translation;
    }
    map< string, string > translats = hit->second;
    map< string, string >::iterator trans = translats.find(language);
    if (trans != translats.end()) {
        translation = trans->second;
    } else {
        map< string, string >::iterator transDefault = translats.find("en_EN");
        if (transDefault != translats.end()) {
            translation = transDefault->second;
        }
    }
    return translation;
}

void cGlobals::AddCustomInt(int num, int value) {
    customIntTokens.erase(num);
    customIntTokens.insert(pair<int,int>(num, value));
    customTokenChange = time(0);
}

void cGlobals::AddCustomString(int num, string &value) {
    customStringTokens.erase(num);
    customStringTokens.insert(pair<int,string>(num, value));
    customTokenChange = time(0);
}

bool cGlobals::GetCustomInt(int num, int &val) {
    map < int, int >::iterator hit = customIntTokens.find(num);
    if (hit != customIntTokens.end()) {
        val = hit->second;
        return true;
    }
    return false;
}

bool cGlobals::GetCustomString(int num, string &val) {
    map < int, string >::iterator hit = customStringTokens.find(num);
    if (hit != customStringTokens.end()) {
        val = hit->second;
        return true;
    }
    return false;
}

map <int, string> cGlobals::GetCustomStringTokens(void) { 
    lastCustomTokenQuery = time(0);
    return customStringTokens;
}

map <int, int> cGlobals::GetCustomIntTokens(void) { 
    lastCustomTokenQuery = time(0);
    return customIntTokens; 
}

bool cGlobals::CustomTokenChange(void) {
    if (!lastCustomTokenQuery)
        return true;
    if (lastCustomTokenQuery < customTokenChange)
        return true;
    return false;
}

void cGlobals::ResetCustomTokenChange(void) {
    lastCustomTokenQuery = 0;
}

void cGlobals::ListCustomTokens(void) {
    for (map<int, string>::iterator it = customStringTokens.begin(); it != customStringTokens.end(); it++) {
        dsyslog("skindesigner: custom string token %d = \"%s\"", it->first, (it->second).c_str());
    }
    for (map<int, int>::iterator it = customIntTokens.begin(); it != customIntTokens.end(); it++) {
        dsyslog("skindesigner: custom int token %d = \"%d\"", it->first, it->second);
    }
}

void cGlobals::Debug(void) {
    dsyslog("skindesigner: GLOBAL VARIABLES");
    for (map <string, tColor>::iterator col = colors.begin(); col != colors.end(); col++) {
        dsyslog("skindesigner: Color \"%s\": %x", (col->first).c_str(), col->second);
    }
    for (map <string, int>::iterator myInt = intVars.begin(); myInt != intVars.end(); myInt++) {
        dsyslog("skindesigner: Integer Variable \"%s\": %d", (myInt->first).c_str(), myInt->second);
    }
    for (map <string, double>::iterator myDouble = doubleVars.begin(); myDouble != doubleVars.end(); myDouble++) {
        dsyslog("skindesigner: Double Variable \"%s\": %f", (myDouble->first).c_str(), myDouble->second);
    }
    for (map <string, string>::iterator myStr = stringVars.begin(); myStr != stringVars.end(); myStr++) {
        dsyslog("skindesigner: String Variable \"%s\": \"%s\"", (myStr->first).c_str(), (myStr->second).c_str());
    }
    for (map <string, string>::iterator font = fonts.begin(); font != fonts.end(); font++) {
        dsyslog("skindesigner: Font \"%s\": \"%s\"", (font->first).c_str(), (font->second).c_str());
    }

    for (map <string, map< string, string > >::iterator trans = translations.begin(); trans != translations.end(); trans++) {
        dsyslog("skindesigner: Translation Token %s", (trans->first).c_str());
        map< string, string > tokenTrans = trans->second;
        for (map< string, string >::iterator transTok = tokenTrans.begin(); transTok != tokenTrans.end(); transTok++) {
            dsyslog("skindesigner: language %s, translation %s", (transTok->first).c_str(), (transTok->second).c_str());
        }
    }
}

void cGlobals::DeleteCurledBrackets(string &token) {
    if (token.find("{") != 0)
        return;
    if (token.find("}") != token.size() - 1)
        return;
    token = token.substr(1, token.size() - 2);
}

