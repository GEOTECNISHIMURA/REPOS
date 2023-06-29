
#ifndef __nakalibH__
#define __nakalibH__


#include <Mstn\MdlApi\MdlApi.h>	// �������W�߂����

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;


/*--- �f�o�b�O�̎��̂݃��b�Z�[�W�o�� ---*/
#ifdef DEBUG
#define debugPrint0( message)                               fprintf( stderr, message)
#define debugPrint1( format, var1)                          fprintf( stderr, format, var1)
#define debugPrint2( format, var1, var2)                    fprintf( stderr, format, var1, var2)
#define debugPrint3( format, var1, var2, var3)              fprintf( stderr, format, var1, var2, var3)
#define debugPrint4( format, var1, var2, var3, var4)        fprintf( stderr, format, var1, var2, var3, var4)
#define debugPrint5( format, var1, var2, var3, var4, var5)  fprintf( stderr, format, var1, var2, var3, var4, var5)
#else
#define debugPrint0( message) 
#define debugPrint1( format, var1) 
#define debugPrint2( format, var1, var2) 
#define debugPrint3( format, var1, var2, var3) 
#define debugPrint4( format, var1, var2, var3, var4) 
#define debugPrint5( format, var1, var2, var3, var4, var5) 
#endif



typedef struct {
	UInt32	lv, co, wt;
	Int32	st;
} ELMSYM;

/*--- ���p���S�p�i�P�����j ---*/
int hantozen(char *zen, char *han);
int hantozen(WCharP zen, WCharP han);

/*--- �S�p�����p�i�P�����j ---*/
int zentohan(char *han, char *zen);
int zentohan(WCharP han, WCharP zen);

/*--- ���C���̋N�_����A���C���ɉ����Đݒ苗���̈ʒu�̍��W�i�ݒ苗����胉�C�����Z������ ERROR�j ---*/
int	get_point_along_line(
	double 		leng, 		/*  => �N�_����̋��� 				*/
	int 		lnv, 		/*  => ����C���̓_�� 			*/
	Dpoint3d 	*lPoi, 		/*  => ����C���̍��W				*/
	int 		*iti, 		/* <=  ����ꂽ���W�̃��C����̈ʒu	*/
	Dpoint3d 	*Poi		/* <=  ����ꂽ���W					*/
);

/*--- ���C����̂Q�_�Ԃ̃��C���ɉ������ݒ苗�����擾 ---*/
void get_leng_along_line(
	double 		*leng, 		/* <=  ���� 			*/
	int 		lnv, 		/*  => ����C���̓_��	*/
	Dpoint3d 	*lPoi, 		/*  => ����C���̍��W	*/
	Dpoint3d 	*Poi1, 		/*  => �_�P�̍��W		*/
	Dpoint3d 	*Poi2		/*  => �_�Q�̍��W		*/
);

/*--- �ϐ��̌��� ---*/
void exchangei(int *ii, int *jj);
void exchangel(long *ii, long *jj);
void exchangeul(ULong *ii, ULong *jj);
void exchanged(double *ii, double *jj);
void exchanged3(Dpoint3d *iPoi, Dpoint3d *jPoi);

/*--- ���C���[��^���ĕ\�� ---*/
void element_display(UInt32 *co, Int32 *lc, UInt32 *wt, MSElement *ele, DgnDrawMode drawMode);

/*--- �l�̌ܓ� ---*/
int sya4_nyu5(int keta, double num1, double *num2);

/*--- �|���S�����E��肩����肩���肷��i�E���:�P�A�����:�Q�j ---*/
int get_round_dir(
	int 		lnv, 	/*  => �|���S���̓_�� 	*/
	Dpoint3d 	*lPoi 	/*  => �|���S���̍��W 	*/
);

/*--- ����_���|���S�����ɂ��邩�ǂ����̔��� 0:�O�A1:���A2:���C���� ---*/
int judge_inout(int *polnv, Dpoint3d *polPoi, Dpoint3d *Poi);

/*--- ���郉�C���ƕʂ̃��C���Ƃ̌�_�i���肷�郉�C����1�ԋN�_���j ---*/
int judge_kouten(
	int *, 		/*  => ���肷�郉�C���̓_�� 		*/
	Dpoint3d *,  /*  => ���肷�郉�C���̊e�_�̍��W 	*/
	int *,  		/*  => �������郉�C���̓_�� 		*/
	Dpoint3d *,  /*  => �������郉�C���̊e�_�̍��W 	*/
	Dpoint3d *   /* <= ��_ 				*/
);

/*--- ���郉�C���ƕʂ̃��C���Ƃ̌�_�A��_����Ԃ� ---*/
int judge_kouten_all(
	int *, 		/*  => ���肷�郉�C���̓_�� 		*/
	Dpoint3d *,  /*  => ���肷�郉�C���̊e�_�̍��W 	*/
	int *,  		/*  => �������郉�C���̓_�� 		*/
	Dpoint3d *,  /*  => �������郉�C���̊e�_�̍��W 	*/
	Dpoint3d *   /* <=  ��_�̊e�_�̍��W 		*/
);

/*--- ����_����1�ԋ߂����C����̓_�Ƃ��̓_�̃��C����ł̈ʒu ---*/
void get_closest_point(
	int *, 		/*  => ���C���̓_�� 					*/
	Dpoint3d *,  /*  => ���C���̊e�_�̍��W 				*/
	Dpoint3d *,  /*  => ���̓_�̍��W 					*/
	Dpoint3d *,  /* <= �P�ԋ߂��_�̍��W 					*/
	int *, 		/* <= �P�ԋ߂��_�̃��C����̈ʒu�i�ǂ̐����ォ�j 		*/
	double *		/* <= �P�ԋ߂��_�̐�����̈ʒu�i�����̋N�_����̋����j 	*/
);

/*--- �d�Ȃ��Ă���_���폜 ---*/
void del_doubled3(int *num, Dpoint3d *Poi, double limit);

/*--- �d�Ȃ��Ă���_���폜�i�t�@�C���ʒu���폜�j ---*/
void del_doubled3_ul(int *num, Dpoint3d *Poi, double limit, ULong *Pos);

/*--- �Q�_�Ԃ̕��ʋ��� ---*/
double get_leng(Dpoint3d *iPoi, Dpoint3d *jPoi);

/*--- �����}�`�̔z�u ---*/
void compShape_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);

/*--- �������C���X�g�����O�̕\�� ---*/
void lineString_display(UInt32 *co, Int32 *lc, UInt32 *wt, Dpoint3d *lPoi, int *nv, DgnDrawMode drawMode);

/*--- �������C���X�g�����O�̔z�u ---*/
void lineString_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);

/*--- ���C���[�ƃO���[�v�ԍ��̎擾 ---*/
void get_layer(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- ���C���[�ƃO���[�v�ԍ��̐ݒ� ---*/
void set_layer(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- �A�����郉�C���X�g�����O�i�܃��C���j�̍��W��Ԃ� ---*/
void get_renzoku(
	int, 		/*  => �����Ώۂ̗v�f�� 					*/
	ULong *, 	/*  => �����Ώۗv�f�̃t�@�C���ʒu 			*/
	int *, 		/* <=> �P�ڂ̗v�f�̓_����n���āA�A���v�f�̓_�� 		*/
	Dpoint3d *, 	/* <=> �P�ڂ̗v�f�̍��W��n���āA�A���v�f�̍��W��Ԃ� */
	int *, 		/* <=> �P��n���āA�A���v�f�̐�							*/
	ULong *		/* <=> �P�ڂ̃t�@�C���ʒu��n���āA�A���v�f�̃t�@�C���ʒu��Ԃ�	*/
);

/*--- �A�����铯�ꃌ�C���[�̃��C���X�g�����O�i�܃��C���j�̍��W��Ԃ� ---*/
void get_renzoku_doulayer(
	int, 		/*  => �����Ώۂ̗v�f�� 					*/
	ULong *, 	/*  => �����Ώۗv�f�̃t�@�C���ʒu 			*/
	int *, 		/* <=> �P�ڂ̗v�f�̓_����n���āA�A���v�f�̓_�� 		*/
	Dpoint3d *, 	/* <=> �P�ڂ̗v�f�̍��W��n���āA�A���v�f�̍��W��Ԃ� */
	int *, 		/* <=> �P��n���āA�A���v�f�̐�							*/
	ULong *		/* <=> �P�ڂ̃t�@�C���ʒu��n���āA�A���v�f�̃t�@�C���ʒu��Ԃ�	*/
);

/*--- �_�����C���i2�_�j�̍��E�ǂ���ɂ��邩�@���F�P�A�E�F�Q ---*/
int judge_point_lr(Dpoint3d *Poi, Dpoint3d *lPoi1, Dpoint3d *lPoi2);

/*--- �_�����C���i2�_�ȏ�j�̍��E�ǂ���ɂ��邩�@���F�P�A�E�F�Q ---*/
int judge_point_lr_lstring(Dpoint3d *Poi, int lnv, Dpoint3d *lPoi);

/*--- �g�^�C�v�ݒ� ---*/
void set_fence_overlap();
void set_fence_inside();
void set_fence_clip();
void set_fence_void_overlap();
void set_fence_void_outside();
void set_fence_void_clip();

/*--- �S��w�\�� ---*/
void on_all();

/*--- �S��w��\�� ---*/
void off_all();

/*--- ��w������wID ---*/
int cnv_level_name2ID(ULong *lvID, WChar *lvName);

/*--- ��wID����w�� ---*/
int cnv_level_ID2name(WChar *lvName, LevelId lvID);

/*--- ��w���ύX ---*/
int chg_levelName(WChar *lvName, LevelId lvID);

/*--- �I�v�V�����{�^���ɉ�w���ꗗ��ǉ� -------------*/
void add_option_lvList(
	MSDialogP	db_p,		/*	=>	�I�v�V�����{�^���̑��݂���_�C�A���O�{�b�N�X	*/
	long 		optionID 	/*	=>	�I�v�V�����{�^��ID		*/
);

/*--- �I�v�V�����{�^���̉�w���ꗗ���폜 -------------*/
void del_option_lvList(
	MSDialogP	db_p,		/*	=>	�I�v�V�����{�^���̑��݂���_�C�A���O�{�b�N�X	*/
	long 		optionID 	/*	=>	�I�v�V�����{�^��ID		*/
);

/*--- �R���{�{�b�N�X�ɉ�w���ꗗ��ǉ� -------------*/
int add_combo_lvList(MSDialogP db_p, long comboID);

/*--- �X�L�����͈̔͂�ݒ� -------------*/
void get_scan_range(ExtScanList *scanlist, Dpoint3d *min, Dpoint3d *max);
void get_scan_range(ScanCriteria *scanP, Dpoint3d *min, Dpoint3d *max);

/*--- ���C���[�ƃO���[�v�ԍ���^���Ĕz�u ---*/
ULong element_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- �~�̍쐬 -------------*/
void create_errCircle(UInt32 lv, UInt32 co, Int32 lc, UInt32 wt, double hankei, Dpoint3d *center);

/*--- ����_�����C�����狖�e�l�ȓ��̋����ɂ��邩�ǂ����̔��� 1:���e�l���A0:���e�l�O ---*/
int	judge_point_nearline(
	int *,		/*  => ���C���̓_�� 		*/
	Dpoint3d *,	/*  => ���C���̊e�_�̍��W 	*/
	Dpoint3d *,	/*  => ���肷��_�̍��W		*/
	double 		/*  => ���e�l 				*/
);

/*--- �}�`���͂ޒ����`�̍����ƉE��̍��W ---*/
void get_min_max(Dpoint3d *min, Dpoint3d *max, int polNv, Dpoint3d *polPoi);

/*--- ���C���[�擾 ---*/
int elmSymbChk(MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr	*eld);

/*--- �����v�f�� MSLINK ���A�^�b�`����BSUCCESS�F�����AERROR�F���s ---*/
ULong attach_mslink_filePos(
	WChar 	*table,  /*  => �f�[�^�x�[�X�e�[�u����		*/
	ULong 	mslink,  /*  => MSLINK						*/
	ULong 	*filePos /*  => �v�f�̃t�@�C���|�W�V����	*/
);

/*--- �V�K�v�f�� MSLINK ���A�^�b�`����BSUCCESS�F�����AERROR�F���s ---*/
ULong attach_mslink_dscr(
	WChar 			*table, /*  => �f�[�^�x�[�X�e�[�u����	*/
	ULong 			mslink, /*  => MSLINK					*/
	MSElementDescr 	*elmDP, /* <=> �v�f						*/
	int		 		flag  	/*  => 0:add ����, 1:add ���Ȃ�	*/
);

/*--- �V�K�v�f�� MSLINK ���A�^�b�`����BSUCCESS�F�����AERROR�F���s ---*/
ULong attach_mslink_ele(
	WChar 		*table, /*  => �f�[�^�x�[�X�e�[�u����	*/
	ULong 		mslink, /*  => MSLINK					*/
	MSElement 	*ele,  	/* <=> �v�f						*/
	int		 	flag  	/*  => 0:add ����, 1:add ���Ȃ�	*/
);

/*-----------------------------------------------------------------------
**	��w�̕\���󋵂��擾����  BoolInt disp[1000];
-----------------------------------------------------------------------*/
void get_display_level_orig(bool *disp);

/*-----------------------------------------------------------------------
**	��w�̕\���󋵂��Z�b�g����@BoolInt disp[1000];
-----------------------------------------------------------------------*/
void set_display_level_orig(bool *disp);


/*------------------------------------------------------------------------------------------------*/
/*--- ���x���}�X�N�̏�Ԃɕ\������ �i�������j-----------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int	set_view_lvmask(short *viewList, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- mdlView_setLevels�̑��� �i�������j--------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int mdlView_setLevels(bool onOffFlag, short *lvmask, short *viewList, int doUpdate);

/*------------------------------------------------------------------------------------------------*/
/*--- ���x���}�X�N����e���x���� ON/OFF��int�z��Ŏ擾 �i�������j---------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_lv_from_lvmask(int *lvOn, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- ���x���}�X�N����e���x���� ON/OFF��bool�z��Ŏ擾 �i�������j---------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_lv_from_lvmask_bool(bool *lvOn, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- ���x���}�X�N�֐ݒ� �i�������j-----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void set_lvmask(UInt32 *lv, short *lvMask);

/*------------------------------------------------------------------------------------------------*/
/*--- �Q�ϐ��̌����ichar�j -----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void exchangechar(WChar *str1, WChar *str2);

/*-----------------------------------------------------------------------
**	�t�@�C���|�W�V����������W���擾
-----------------------------------------------------------------------*/
void get_point_from_Pos(int *nv, Dpoint3d *Poi, ULong Pos);

/*------------------------------------------------------------------------------------------------*/
/*--- �Q�{�̃��C���A���C���X�g�����O�̕��������킹�� ---------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void set_dir(int nv1, Dpoint3d *Poi1, int nv2, Dpoint3d *Poi2);

/*-----------------------------------------------------------------------
**	���C�����t�����ɂ���
-----------------------------------------------------------------------*/
void reverse(int nv, Dpoint3d *Poi);

/*-----------------------------------------------------------------------
**	mdlElement_display �̑���
-----------------------------------------------------------------------*/
void mdlElement_displayCE(MSElementP elm, DgnDrawMode drawMode);

/*-----------------------------------------------------------------------
**	mdlElmdscr_display �̑���
-----------------------------------------------------------------------*/
void mdlElmdscr_displayCE(MSElementDescrP elmDesc, DgnModelRefP modelRefP, DgnDrawMode drawMode);

/*-----------------------------------------------------------------------
**	�ꎞ�`��v�f�̍폜
-----------------------------------------------------------------------*/
void mdlTransient_free2(bool eraseDisplay);







// �ȉ��Anaka.h���ڐA ----------------------------------------------------------------------------------------------

/*------------------------------------------------------------------------------------------------*/
/*--- �z��ɕϐ���ǉ� (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int judge_point_within_range(int iti1, double leng1, int iti, double leng, int iti2, double leng2);

/*------------------------------------------------------------------------------------------------*/
/*--- �z��ɕϐ���ǉ� (Dpoint3d)(����) ----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_d3_increase(Dpoint3d *addPoi, int start, int end, Dpoint3d *oriPoi, int *nv);

/*------------------------------------------------------------------------------------------------*/
/*--- �z��ɕϐ���ǉ� (Dpoint3d)(����) ----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_d3_decrease(Dpoint3d *addPoi, int start, int end, Dpoint3d *oriPoi, int *nv);

/*------------------------------------------------------------------------------------------------*/
/*--- �z��ɕϐ���ǉ� (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_variable_to_arrayd3(Dpoint3d *addVar, Dpoint3d *chgArray, int *arrayNum);

/*------------------------------------------------------------------------------------------------*/
/*--- �z��ɔz���ǉ� (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_to_arrayd3(Dpoint3d *addArray, int start, int end, Dpoint3d *chgArray, int *arrayNum);

/*------------------------------------------------------------------------------------------------*/
/*--- �|���S���̍쐬 -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void polygon_create(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int nv, MSElementDescr **elmDP);

/*------------------------------------------------------------------------------------------------*/
/*--- ��_�Ƃ��̈ʒu -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_kouten_iti(int nv1, Dpoint3d *Poi1, int nv2, Dpoint3d *Poi2, Dpoint3d *nearPoi, Dpoint3d *kouPoi, int *iti, double *leng);

/*-----------------------------------------------------------------------
/*	���C���̒��_���擾
/*---------------------------------------------------------------------*/
void get_line_tyuuten(Dpoint3d *mPoi, int nv, Dpoint3d *lPoi);

/*-----------------------------------------------------------------------
/*	�Q�_�Ԃ̒��_���擾
/*---------------------------------------------------------------------*/
void get_tyuuten(Dpoint3d *mPoi, Dpoint3d *lPoi1, Dpoint3d *lPoi2);

/*---------------------------------------------------------------------------------------------------------*/
/*--- ��������w�肵�������̕�����ɕϊ��i�������]�鎞�ɂ͓��ɃX�y�[�X������j --------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int ttostxt(int keta, WChar *txt1, WChar *txt2);

/*---------------------------------------------------------------------------------------------------------*/
/*--- ��������w�肵�������̕�����ɕϊ��i�������]�鎞�ɂ͓���0������j ---------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int tto0txt(int keta, WChar *txt1, WChar *txt2);

/*------------------------------------------------------------------------------------------------*/
/*--- ���l���w�肵�������̕�����ɕϊ��i�������]�鎞�ɂ͓��ɃX�y�[�X������j -------------------*/
/*------------------------------------------------------------------------------------------------*/
int itostxt(int keta, int num, WChar *txt, int flag);

/*------------------------------------------------------------------------------------------------*/
/*--- ���l���w�肵�������̕�����ɕϊ��i�������]�鎞�ɂ͓���0������j --------------------------*/
/*------------------------------------------------------------------------------------------------*/
int ito0txt(int keta, int num, WChar *txt);





// �ȉ��Anakadb.h���ڐA ----------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------
** �f�[�^�x�[�X�e�[�u���̏㏑���R�s�[
-----------------------------------------------------------------------*/
void drop_table(WChar *copy, WChar *orig);

/*-----------------------------------------------------------------------
**	SQL���̎��s
-----------------------------------------------------------------------*/
int process_SQL(WChar *ins);

/*-----------------------------------------------------------------------
**	�v�f�����Ƀ����N�ς݂��ǂ�����������
-----------------------------------------------------------------------*/
int check_mslink(MSElement *ele, ULong *mslink);


// ODBC�Őڑ����Ă��邩
bool IsOdbcConnection();



#endif


