//////////////////////////////////////////////////////////////////////////////
// WordData.cpp --- XWord Giver (Japanese Crossword Generator)
// Copyright (C) 2012-2019 Katayama Hirofumi MZ. All Rights Reserved.
// (Japanese, Shift_JIS)

#include "XWordGiver.hpp"

// �����f�[�^�B
std::vector<XG_WordData>     xg_dict_data;

// �����̃w�b�_�[�B
static std::wstring s_header = L"";

bool s_bDictModified = false;

//////////////////////////////////////////////////////////////////////////////
// �����f�[�^�̃t�@�C�������B

// Unicode����s�ǂݍ��ށB
void XgReadUnicodeLine(LPWSTR pchLine)
{
    XG_WordData entry;
    std::array<WCHAR,64> szWord;

    // �R�����g�s��ǂݎ��B
    if (*pchLine == L'#') {
        s_header += pchLine;
        s_header += xg_pszNewLine;
        return;
    }

    // ��s�𖳎�����B
    if (*pchLine == L'\0') {
        return;
    }

    // �q���g���������B
    LPWSTR pchHint = wcschr(pchLine, L'\t');
    if (pchHint) {
        *pchHint = 0;
        pchHint++;
    } else {
        pchHint = nullptr;
    }

    // �P�ꕶ�����S�p�E�J�^�J�i�E�啶���ɂ���B
    LCMapStringW(JPN_LOCALE,
        LCMAP_FULLWIDTH | LCMAP_KATAKANA | LCMAP_UPPERCASE,
        pchLine, static_cast<int>(wcslen(pchLine) + 1), szWord.data(), 64);

    // ������̑O��̋󔒂���菜���B
    entry.m_word = szWord.data();
    xg_str_trim(entry.m_word);

    // �����Ȏ���傫�Ȏ��ɂ���B
    for (size_t i = 0; i < xg_large.size(); i++)
        xg_str_replace_all(entry.m_word,
            std::wstring(xg_small[i]), std::wstring(xg_large[i]));

    // �P��ƃq���g��o�^����B�ꎚ�ȉ��̒P��͓o�^���Ȃ��B
    if (entry.m_word.size() > 1) {
        if (pchHint) {
            entry.m_hint = pchHint;
            xg_str_trim(entry.m_hint);
        }
        else
            entry.m_hint.clear();

        xg_dict_data.emplace_back(std::move(entry));
    }
}

// Unicode�̃t�@�C���̒��g��ǂݍ��ށB
bool XgReadUnicodeFile(LPWSTR pszData, DWORD /*cchData*/)
{
    // �ŏ��̈�s�����o���B
    LPWSTR pchLine = wcstok(pszData, xg_pszNewLine);
    if (pchLine == nullptr)
        return false;

    // ��s����������B
    do {
        XgReadUnicodeLine(pchLine);
        pchLine = wcstok(nullptr, xg_pszNewLine);
    } while (pchLine);
    return true;
}

// ANSI (Shift_JIS) �̃t�@�C���̒��g��ǂݍ��ށB
bool __fastcall XgReadAnsiFile(LPCSTR pszData, DWORD /*cchData*/)
{
    // Unicode�ɕϊ��ł��Ȃ��Ƃ��͎��s�B
    int cchWide = MultiByteToWideChar(932, 0, pszData, -1, nullptr, 0);
    if (cchWide == 0)
        return false;

    // Unicode�ɕϊ����ď�������B
    LPWSTR pszWide = new WCHAR[cchWide];
    MultiByteToWideChar(932, 0, pszData, -1, pszWide, cchWide);
    if (XgReadUnicodeFile(pszWide, cchWide - 1)) {
        // �����B
        delete[] pszWide;
        return true;
    }

    // ���s�B
    delete[] pszWide;
    return false;
}

// UTF-8�̃t�@�C���̒��g��ǂݍ��ށB
bool __fastcall XgReadUtf8File(LPCSTR pszData, DWORD /*cchData*/)
{
    // Unicode�ɕϊ��ł��Ȃ��Ƃ��͎��s�B
    int cchWide = MultiByteToWideChar(CP_UTF8, 0, pszData, -1, nullptr, 0);
    if (cchWide == 0)
        return false;

    // Unicode�ɕϊ����ď�������B
    LPWSTR pszWide = new WCHAR[cchWide];
    MultiByteToWideChar(CP_UTF8, 0, pszData, -1, pszWide, cchWide);
    if (XgReadUnicodeFile(pszWide, cchWide - 1)) {
        // �����B
        delete[] pszWide;
        return true;
    }

    // ���s�B
    delete[] pszWide;
    return false;
}

// �����t�@�C����ǂݍ��ށB
bool __fastcall XgLoadDictFile(LPCWSTR pszFile)
{
    DWORD cbRead, i;
    LPBYTE pbFile = nullptr;

    // ����������B
    xg_dict_data.clear();
    s_header.clear();

    // �t�@�C�����J���B
    HANDLE hFile = CreateFileW(pszFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // �t�@�C���T�C�Y���擾����B
    DWORD cbFile = ::GetFileSize(hFile, nullptr);
    if (cbFile == 0xFFFFFFFF)
        return false;

    try {
        // ���������m�ۂ��ăt�@�C������ǂݍ��ށB
        pbFile = new BYTE[cbFile + 3];
        i = cbFile;
        if (ReadFile(hFile, pbFile, cbFile, &cbRead, nullptr)) {
            // BOM�`�F�b�N�B
            if (pbFile[0] == 0xFF && pbFile[1] == 0xFE) {
                // Unicode
                pbFile[cbFile] = 0;
                pbFile[cbFile + 1] = 0;
                pbFile[cbFile + 2] = 0;
                std::wstring str = reinterpret_cast<LPWSTR>(&pbFile[2]);
                if (!XgReadUnicodeFile(const_cast<LPWSTR>(str.data()),
                                     static_cast<DWORD>(str.size())))
                    throw 0;
                i = 0;
            } else if (pbFile[0] == 0xFE && pbFile[1] == 0xFF) {
                // Unicode BE
                pbFile[cbFile] = 0;
                pbFile[cbFile + 1] = 0;
                pbFile[cbFile + 2] = 0;
                XgSwab(pbFile, cbFile);
                std::wstring str = reinterpret_cast<LPWSTR>(&pbFile[2]);
                if (!XgReadUnicodeFile(const_cast<LPWSTR>(str.data()),
                                     static_cast<DWORD>(str.size())))
                    throw 0;
                i = 0;
            } else if (pbFile[0] == 0xEF && pbFile[1] == 0xBB && pbFile[2] == 0xBF) {
                // UTF-8
                pbFile[cbFile] = 0;
                std::wstring str = XgUtf8ToUnicode(reinterpret_cast<LPCSTR>(&pbFile[3]));
                if (!XgReadUnicodeFile(const_cast<LPWSTR>(str.data()),
                                     static_cast<DWORD>(str.size())))
                    throw 0;
                i = 0;
            } else {
                for (i = 0; i < cbFile; i++) {
                    // �i�������������Unicode�Ɣ��f����B
                    if (pbFile[i] == 0) {
                        pbFile[cbFile] = 0;
                        pbFile[cbFile + 1] = 0;
                        pbFile[cbFile + 2] = 0;
                        if (i & 1) {
                            // Unicode
                            if (!XgReadUnicodeFile(reinterpret_cast<LPWSTR>(pbFile), cbFile / 2))
                                throw 0;
                        } else {
                            // Unicode BE
                            XgSwab(pbFile, cbFile);
                            if (!XgReadUnicodeFile(reinterpret_cast<LPWSTR>(pbFile), cbFile / 2))
                                throw 0;
                        }
                        i = 0;
                        break;
                    }
                }
            }
            if (i == cbFile) {
                pbFile[cbFile] = 0;
                if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        reinterpret_cast<LPCSTR>(pbFile),
                                        static_cast<int>(cbFile), nullptr, 0))
                {
                    // UTF-8
                    if (!XgReadUtf8File(reinterpret_cast<LPSTR>(pbFile), cbFile))
                        throw 0;
                } else {
                    // ANSI
                    if (!XgReadAnsiFile(reinterpret_cast<LPSTR>(pbFile), cbFile))
                        throw 0;
                }
            }

            // �񕪒T���̂��߂ɁA���ёւ��Ă����B
            std::sort(xg_dict_data.begin(), xg_dict_data.end(), xg_word_less());

            // �����B
            delete[] pbFile;
            CloseHandle(hFile);
            return true;
        }
    } catch (...) {
        // ��O�����������B
    }

    // ���s�B
    delete[] pbFile;
    CloseHandle(hFile);
    return false;
}

// ��������P���T���o���B
XG_WordData *XgFindWordFromDict(const std::wstring& word)
{
    for (size_t i = 0; i < xg_dict_data.size(); ++i) {
        if (xg_dict_data[i].m_word == word) {
            return &xg_dict_data[i];
        }
    }
    return NULL;
}

// �����ɍX�V�����邩�ǂ����H
bool __fastcall XgIsDictUpdated(void)
{
    // �q���g�ǉ��t���O���ݒ肳��Ă���ꍇ�́A�ύX���ꂽ�B
    if (xg_bHintsAdded || s_bDictModified) {
        return true;
    }

    // �����ɂ��̒P�ꂪ���݂��Ȃ����A�q���g���Ⴄ�ꍇ�́A�X�V���ꂽ�B
    LPTSTR pszTate = XgLoadStringDx1(90);
    for (auto& word_data : xg_vecTateHints) {
        // �u�^�e�v���܂ރq���g�͖�������B
        if (word_data.m_strHint.find(pszTate) != std::wstring::npos)
            continue;

        XG_WordData *data = XgFindWordFromDict(word_data.m_strWord);
        if (data == NULL) {
            s_bDictModified = true;
            return true;
        }
        if (data->m_hint != word_data.m_strHint) {
            s_bDictModified = true;
            return true;
        }
    }
    LPTSTR pszYoko = XgLoadStringDx1(91);
    for (auto& word_data : xg_vecYokoHints) {
        // �u���R�v���܂ރq���g�͖�������B
        if (word_data.m_strHint.find(pszYoko) != std::wstring::npos)
            continue;

        XG_WordData *data = XgFindWordFromDict(word_data.m_strWord);
        if (data == NULL) {
            s_bDictModified = true;
            return true;
        }
        if (data->m_hint != word_data.m_strHint) {
            s_bDictModified = true;
            return true;
        }
    }

    // �X�V����Ă��Ȃ��B
    return false;
}

// �����ɍX�V�����邩�ǂ����H
void __fastcall XgDictSetModified(bool modified)
{
    s_bDictModified = modified;
}

// �����f�[�^���X�V����B
bool __fastcall XgUpdateDictData(void)
{
    bool updated = false;
    LPTSTR pszTate = XgLoadStringDx1(90);
    for (auto& word_data : xg_vecTateHints) {
        if (word_data.m_strHint.find(pszTate) != std::wstring::npos)
            continue;

        XG_WordData *data = XgFindWordFromDict(word_data.m_strWord);
        if (data == NULL) {
            xg_dict_data.emplace_back(word_data.m_strWord,
                                      word_data.m_strHint);
            updated = true;
            continue;
        }
        if (data->m_hint != word_data.m_strHint) {
            data->m_hint = word_data.m_strHint;
            updated = true;
        }
    }
    LPTSTR pszYoko = XgLoadStringDx1(91);
    for (auto& word_data : xg_vecYokoHints) {
        if (word_data.m_strHint.find(pszYoko) != std::wstring::npos)
            continue;

        XG_WordData *data = XgFindWordFromDict(word_data.m_strWord);
        if (data == NULL) {
            xg_dict_data.emplace_back(word_data.m_strWord,
                                      word_data.m_strHint);
            updated = true;
            continue;
        }
        if (data->m_hint != word_data.m_strHint) {
            data->m_hint = word_data.m_strHint;
            updated = true;
        }
    }
    if (updated) {
        XgSortAndUniqueDictData();
        s_bDictModified = true;
    }
    return updated;
}

// �����f�[�^���\�[�g���A��ӓI�ɂ���B
void __fastcall XgSortAndUniqueDictData(void)
{
    sort(xg_dict_data.begin(), xg_dict_data.end(), xg_word_less());
    auto last = unique(xg_dict_data.begin(), xg_dict_data.end(),
                       XG_WordData_Equal());
    std::vector<XG_WordData> dict_data;
    for (auto it = xg_dict_data.begin(); it != last; ++it) {
        dict_data.emplace_back(*it);
    }
    xg_dict_data = std::move(dict_data);
}
