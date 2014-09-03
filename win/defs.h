#include        <windows.h>
#include        <stdio.h>
#include        <strsafe.h>
#include        <AccCtrl.h>
#include        <sddl.h>
#include        <aclapi.h>
#include        "pe64.h"


typedef BOOLEAN (WINAPI *WOW64ENABLEWOW64FSREDIRECTION)(
  __in  BOOLEAN Wow64FsEnableRedirection
);

typedef BOOL (WINAPI *ISWOW64PROCESS)(
  __in   HANDLE hProcess,
  __out  PBOOL Wow64Process
);

