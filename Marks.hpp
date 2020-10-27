//////////////////////////////////////////////////////////////////////////////
// Marks.hpp --- XWord Giver (Japanese Crossword Generator)
// Copyright (C) 2012-2020 Katayama Hirofumi MZ. All Rights Reserved.
// (Japanese, Shift_JIS)

#ifndef __XWORDGIVER_MARKS_HPP__
#define __XWORDGIVER_MARKS_HPP__

extern std::vector<XG_Pos> xg_vMarks;

//////////////////////////////////////////////////////////////////////////////

// ��d�}�X�P����B
extern std::vector<std::wstring>  xg_vMarkedCands;

//////////////////////////////////////////////////////////////////////////////

// �}�[�N��������擾����B
void __fastcall XgGetStringOfMarks(std::wstring& str);
// �}�[�N����Ă��邩�i��d�}�X�j�H
int __fastcall XgGetMarked(const std::vector<XG_Pos>& vMarks, const XG_Pos& pos);
// �}�[�N����Ă��邩�i��d�}�X�j�H
int __fastcall XgGetMarked(const std::vector<XG_Pos>& vMarks, int i, int j);
// �}�[�N����Ă��邩�i��d�}�X�j�H
int __fastcall XgGetMarked(int i, int j);
// ��d�}�X���X�V���ꂽ�B
void __fastcall XgMarkUpdate(void);
// �w��̃}�X�Ƀ}�[�N����i��d�}�X�j�B
void __fastcall XgSetMark(const XG_Pos& pos);
// �w��̃}�X�Ƀ}�[�N����i��d�}�X�j�B
void __fastcall XgSetMark(int i, int j);
// �w��̃}�X�̃}�[�N�i��d�}�X�j����������B
void __fastcall XgDeleteMark(int i, int j);
// �}�[�N�������ݒ肷��B
void __fastcall XgSetStringOfMarks(LPCWSTR psz);
// ��d�}�X�P����擾����B
bool __fastcall XgGetMarkWord(const XG_Board *xw, std::wstring& str);
// ��d�}�X�P���ݒ肷��B
void __fastcall XgSetMarkedWord(const std::wstring& str);
// ��d�}�X�P�����ɂ���B
void __fastcall XgSetMarkedWord(void);
// ���̓�d�}�X�P����擾����B
void __fastcall XgGetNextMarkedWord(void);
// �O�̓�d�}�X�P����擾����B
void __fastcall XgGetPrevMarkedWord(void);

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef __XWORDGIVER_MARKS_HPP__
