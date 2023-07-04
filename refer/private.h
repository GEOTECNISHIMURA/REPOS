/*----------------------------------------------------------------------+
|ユーザー定義関数のプロトタイプ宣言                                    	|
+----------------------------------------------------------------------*/
Private void	Jikou_Ok(DialogItemMessage *dmP);
Private void	Bat_Name(DialogItemMessage *dmP);
Private void    dialog_hook(DialogMessage *dmP);

Private DialogHookInfo uHooks[] =
	{
{HOOKITEMID_Jikou,		    (PFDialogHook)Jikou_Ok},
{HOOKITEMID_BatName,		(PFDialogHook)Bat_Name},
{HOOKID_REFER_DIALOG,		(PFDialogHook)dialog_hook },
	};

