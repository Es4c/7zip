// PasswordDialog.h

#ifndef ZIP7_INC_PASSWORD_DIALOG_H
#define ZIP7_INC_PASSWORD_DIALOG_H

#include "../../../Windows/Control/Dialog.h"
#include "../../../Windows/Control/Edit.h"

#include "PasswordDialogRes.h"

class CPasswordDialog: public NWindows::NControl::CModalDialog
{
  NWindows::NControl::CEdit _passwordEdit;

  virtual void OnOK() Z7_override;
  virtual bool OnInit() Z7_override;
  virtual bool OnButtonClicked(unsigned buttonID, HWND buttonHWND) Z7_override;
  void SetTextSpec();
  void ReadControls();
public:
  UString Password;
  bool ShowPassword;
  bool ApplyToRestVisible; // in : show "apply to remaining archives" checkbox
  bool ApplyToRest;        // in/out : state of that checkbox
  
  CPasswordDialog(): ShowPassword(false), ApplyToRestVisible(false), ApplyToRest(false) {}
  INT_PTR Create(HWND parentWindow = NULL) { return CModalDialog::Create(IDD_PASSWORD, parentWindow); }
};

#endif
