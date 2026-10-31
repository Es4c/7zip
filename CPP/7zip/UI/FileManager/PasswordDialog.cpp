// PasswordDialog.cpp

#include "StdAfx.h"

#include "PasswordDialog.h"

#ifdef Z7_LANG
#include "LangUtils.h"
#endif

#ifdef Z7_LANG
static const UInt32 kLangIDs[] =
{
  IDT_PASSWORD_ENTER,
  IDX_PASSWORD_SHOW,
  IDX_PASSWORD_APPLY_REST
};
#endif

void CPasswordDialog::ReadControls()
{
  _passwordEdit.GetText(Password);
  ShowPassword = IsButtonCheckedBool(IDX_PASSWORD_SHOW);
  ApplyToRest = ApplyToRestVisible && IsButtonCheckedBool(IDX_PASSWORD_APPLY_REST);
}

void CPasswordDialog::SetTextSpec()
{
  _passwordEdit.SetPasswordChar(ShowPassword ? 0: TEXT('*'));
  _passwordEdit.SetText(Password);
}

bool CPasswordDialog::OnInit()
{
  #ifdef Z7_LANG
  LangSetWindowText(*this, IDD_PASSWORD);
  LangSetDlgItems(*this, kLangIDs, Z7_ARRAY_SIZE(kLangIDs));
  #endif
  _passwordEdit.Attach(GetItem(IDE_PASSWORD_PASSWORD));
  CheckButton(IDX_PASSWORD_SHOW, ShowPassword);
  CheckButton(IDX_PASSWORD_APPLY_REST, ApplyToRest);
  SetTextSpec();

  #ifndef UNDER_CE
  if (!ApplyToRestVisible)
  {
    /* the checkbox is not needed (single archive or the last archive):
       we hide it and remove its row, so the dialog looks as before. */
    HideItem(IDX_PASSWORD_APPLY_REST);
    RECT r;
    r.left = 0; r.top = 0; r.right = 4; r.bottom = 14; // height of the row in dialog units
    ::MapDialogRect(*this, &r);
    const int delta = r.bottom;
    const unsigned ids[] = { IDOK, IDCANCEL };
    for (unsigned i = 0; i < Z7_ARRAY_SIZE(ids); i++)
    {
      RECT rect;
      GetClientRectOfItem(ids[i], rect);
      MoveItem(ids[i], rect.left, rect.top - delta, RECT_SIZE_X(rect), RECT_SIZE_Y(rect));
    }
    RECT wr;
    if (GetWindowRect(&wr))
      Move(wr.left, wr.top, RECT_SIZE_X(wr), RECT_SIZE_Y(wr) - delta);
  }
  #endif

  return CModalDialog::OnInit();
}

bool CPasswordDialog::OnButtonClicked(unsigned buttonID, HWND buttonHWND)
{
  if (buttonID == IDX_PASSWORD_SHOW)
  {
    ReadControls();
    SetTextSpec();
    return true;
  }
  return CDialog::OnButtonClicked(buttonID, buttonHWND);
}

void CPasswordDialog::OnOK()
{
  ReadControls();
  CModalDialog::OnOK();
}
