#pragma once

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 90.f };
	static const size_t max_bullet{ 64 };

	enum class Effect {
		ef_fire,	//î≠ñCâä
		ef_reco,	//è¨å˚åaíµíe
		ef_smoke,	//èeÇÃãOê’
		ef_gndsmoke,//ínñ ÇÃãOê’
		ef_fire2,	//î≠ñCâä
		ef_hitblood,//ååÇµÇ‘Ç´
		ef_greexp,	//ÉOÉåîöî≠
		effects,	//ì«Ç›çûÇﬁ
	};
	enum class EnumGunParts {
		PARTS_NONE,
		PARTS_BASE,
		PARTS_MAGAZINE,
		PARTS_MAZZULE,
		PARTS_GRIP,
		PARTS_UPER_HGUARD,
		PARTS_UNDER_HGUARD,
		PARTS_DUSTCOVER,
		PARTS_STOCK,
		PARTS_LAM,
		PARTS_MOUNT_BASE,
		PARTS_MOUNT,
		PARTS_SIGHT,
		PARTS_FOREGRIP,
		PARTS_GUN,
	};
	enum class EnumAttachPoint {
		POINTS_NONE,
		POINTS_UPER_RAIL,
		POINTS_UNDER_RAIL,
		POINTS_LEFTSIDE_RAIL,
		POINTS_RIGHTSIDE_RAIL,
		POINTS_SIDEMOUNT,
		POINTS_SIDEMOUNT_BASE,
		POINTS_STOCK_BASE,
		POINTS_DUSTCOVER_BASE,
		POINTS_UPER_HANDGUARD,
		POINTS_UNDER_HANDGUARD,
		POINTS_MAZZULE_BASE,
		POINTS_MAGAZINE_BASE,
		POINTS_GRIP_BASE,
		POINTS_NUM,
	};
	enum class EnumSELECTER {
		SELECT_SEMI,
		SELECT_FULL,
		SELECT_3B,
		SELECT_2B,
		SELECT_NUM,
	};
	enum class EnumSELECT_LAM {
		SELECTLAM_LASER,
		SELECTLAM_LIGHT,
		SELECTLAM_NUM,
	};
	enum class scenes
	{
		NONE_SCENE,
		ITEM_LOAD,
		MAP_LOAD,
		LOAD,
		SELECT,
		MAIN_LOOP
	};

	enum class EnumKeyBind {
		FRONT,
		BACK,
		RIGHT,
		LEFT,
		Q,
		E,
		RELOAD,
		GET_ITEM,
		THROW_GRE,
		SIT,
		ESCAPE,
		SORT_MAG,
		RUN,
		JUMP,
		CHANGE_VIEW,
		INFO,
		LOOKGUN,
		PAUSE,
		MED,
		RIDE_ON,
	};

	enum class EnumMouseBind {
		SHOOT,
		CHANGE_SEL,
		AIM,
	};

	enum class EnumSound {
		//UI

		CURSOR,
		CANCEL,
		TIMER,

		//human

		Sort_MAG,
		Cate_Load,
		Foot_Sound,
		Explosion,

		Voice_Damage,
		Voice_Death,
		Voice_Breath,
		Voice_Breath_Run,

		//Gun

		Assemble,
		Shot,
		Slide,
		Trigger,
		MAG_Down,
		MAG_Set,
		Cate_Down,

		//Tank

		Tank_Damage,
		Tank_Shot,
		Tank_Reload,
		Tank_Ricochet,
		Tank_engine,

		//Item

		//MAP
		MAP0_ENVI,
	};
}
