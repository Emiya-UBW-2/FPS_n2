#pragma once

#include <cassert>
#include <memory>

#include "sub.hpp"

class PLAYERclass : Mainclass{


public:
	class vehicles;
	class Chara;

	class PLAYER_COMMON {
		class damage_rad {
		public:
			float alpfa{ 1.f };
			float rad{ 0.f };
		};
	protected:
		std::shared_ptr<MAPclass::Map> MAPPTs{ nullptr };	//引き継ぐ
		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//引き継ぐ
		moves move;											//プレイヤー座標
		MV1 obj_body;										//本体オブジェ
		MV1 obj_col;										//本体コリジョン
		float speed{ 0.f };								//移動速度
	public:
		//HPバー
		unsigned int got_damage_color{ 0 };
		int got_damage_x{ 0 };
		float got_damage_f{ 0.f };
		std::vector<damage_rad> got_damage_;
		int got_damage{ 0 };
		int HP{ 100 };
		float HP_r{ 100.f };
		int HP_full{ 100 };
		std::vector<int> HP_parts;
	public:
		const auto& get_move() const noexcept { return this->move; }

		/*コンストラクタ*/
		PLAYER_COMMON(std::shared_ptr<MAPclass::Map>& MAPPTs_t, std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
			DrawPts = DrawPts_t;
			MAPPTs = MAPPTs_t;
		}

		const MV1_COLL_RESULT_POLY col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel) const noexcept {
			return obj_col.CollCheck_Line(StartPos, EndPos, -1, sel);
		}
	};

	class Chara : public std::enable_shared_from_this<Chara>, public PLAYER_COMMON {
	public:
		//銃に着けるパーツ
		class GPARTS_COMMON {
		protected:
			//共通
			bool attach{ false };										//取り付いているか
			EnumGunParts type{ PARTS_NONE };							//パーツの種類
			VECTOR_ref vec_parts;										//根元からのベクトル
			std::array<std::array<frames, 2>, POINTS_NUM> rail_frame;	//アタッチできるポイント
			frames rad_parts_frame;										//傾き
			frames attach_frame;										//根元
			MV1 obj;													//モデル
		public:
			GUNPARTs* thisparts{ nullptr };	//自身のデータ
			GPARTS_COMMON* parent{ nullptr };		//親パーツ
		public:
			const auto& IsActive(void) const noexcept				 { return this->attach; }
			const auto& Get_type(void) const noexcept				 { return this->type; }
			const auto& Get_rail_frame(size_t sel_t) const noexcept	 { return this->rail_frame[sel_t][0]; }
			const auto& Get_attach_frame(void) const noexcept		 { return this->attach_frame.second; }
			const auto Get_rail_pos(size_t sel_t) const noexcept	 { return (IsActive()) ? this->obj.frame(this->rail_frame[sel_t][0].first) : VECTOR_ref::vget(0, 0, 0); }
			//フレームリセット
			virtual void ReSet_frame(void) noexcept {
				for (auto& r : this->rail_frame) {
					r[0].first = -1;
					r[1].first = -1;
				}
				this->rad_parts_frame.first = -1;
				this->attach_frame.first = -1;
			}
			virtual void get_frame_(GUNPARTs& parts, EnumGunParts type_t) noexcept {
				if (IsActive()) {
					this->ReSet_frame();
					this->thisparts = &parts;
					this->type = type_t;
					SetUseASyncLoadFlag(FALSE);
					if (this->type == PARTS_BASE) {
						this->thisparts->mod.get_model().DuplicateonAnime(&this->obj, &this->obj);
					}
					else {
						this->obj = this->thisparts->mod.get_model().Duplicate();
					}
					for (int i = 0; i < this->obj.frame_num(); ++i) {
						std::string p = this->obj.frame_name(i);
						if (p == "rail") {
							this->rail_frame[POINTS_UPER_RAIL][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_UPER_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "underrail") {
							this->rail_frame[POINTS_UNDER_RAIL][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_UNDER_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "siderail") {
							this->rail_frame[POINTS_LEFTSIDE_RAIL][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_LEFTSIDE_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "rightrail") {
							this->rail_frame[POINTS_RIGHTSIDE_RAIL][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_RIGHTSIDE_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "sidemount") {
							this->rail_frame[POINTS_SIDEMOUNT][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_SIDEMOUNT][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "sidemount_base") {
							this->rail_frame[POINTS_SIDEMOUNT_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_SIDEMOUNT_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "stock") {
							this->rail_frame[POINTS_STOCK_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_STOCK_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "dustcover") {
							this->rail_frame[POINTS_DUSTCOVER_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_DUSTCOVER_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "uper_handguard") {
							this->rail_frame[POINTS_UPER_HANDGUARD][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_UPER_HANDGUARD][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "under_handguard") {
							this->rail_frame[POINTS_UNDER_HANDGUARD][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_UNDER_HANDGUARD][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "mazzule") {
							this->rail_frame[POINTS_MAZZULE_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_MAZZULE_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "grip") {
							this->rail_frame[POINTS_GRIP_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_GRIP_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
						else if (p == "rad_parts") {
							this->rad_parts_frame = { int(i),this->obj.frame(i) };
						}
						else if (p == "magazine_fall") {
							this->rail_frame[POINTS_MAGAZINE_BASE][0] = { int(i),this->obj.frame(i) };
							this->rail_frame[POINTS_MAGAZINE_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
						}
					}
				}
			}
			//描画
			virtual void Draw(void) noexcept {
				if (IsActive()) {
					this->obj.DrawModel();
				}
			}
			//パーツを外す
			virtual void Detach(performance& per_all) noexcept {
				if (IsActive()) {
					this->attach = false;
					this->ReSet_frame();
					per_all -= this->thisparts->per;
					this->thisparts = nullptr;
					this->parent = nullptr;
					this->type = PARTS_NONE;
					this->obj.Dispose();
				}
			}
			//ベース用座標指定
			virtual void Setpos_Nomal(const MATRIX_ref& mat_t) noexcept {
				if (IsActive()) {
					this->obj.SetMatrix(mat_t);
				}
			}
			//パーツ用の座標指定
			virtual void Setpos_parts(const MATRIX_ref& mat_t) noexcept {
				if (IsActive()) {
					this->obj.SetMatrix((MATRIX_ref::RotVec2(this->rad_parts_frame.second.Norm(), this->vec_parts) * mat_t) * MATRIX_ref::Mtrans(this->parent->obj.frame(this->attach_frame.first)));
				}
			}
			//アニメ更新
			virtual void UpDate_Anim(void) { this->obj.work_anime(); }
			//便利系
			virtual const MATRIX_ref Get_objMatrix(void) const noexcept { return this->obj.GetMatrix(); }
			virtual const MATRIX_ref Get_objFrameLocalMatrix(size_t sel_t) const noexcept { return obj.GetFrameLocalMatrix(int(sel_t)); }
			virtual MV1::ani* Get_Anim(size_t sel_t) noexcept { return &this->obj.get_anime(sel_t); }
		protected:
			//親を設定する
			void set_parent(GPARTS_COMMON* parents, size_t side) {
				this->parent = parents;
				this->attach_frame = parents->rail_frame[side][0];
				this->vec_parts = parents->rail_frame[side][1].second.Norm();
			}
			//親の座標を探す
			void search_parts(EnumGunParts PARTS_, VECTOR_ref* pv, const GPARTS_COMMON* ptr) noexcept {
				if (ptr->parent->Get_type() != PARTS_ && ptr->parent != nullptr) {
					*pv += ptr->parent->Get_attach_frame();
					search_parts(PARTS_BASE, pv, ptr->parent);
				}
			}
		};
		//銃に着けるパーツ詳細版
		class g_parts:public GPARTS_COMMON {
		private:
			/*汎用的専用*/
			frames mag_frame;						//マガジン
			frames cate_frame;						//排莢
			frames slide_frame;						//スライド
			std::array<frames, 3> RIGHT_frame;		//右手座標
			std::array<frames, 3> LEFT_frame;		//左手座標
			std::array<frames, 3> LEFT_mag_frame;	//左手座標(マガジン保持時)
			/*LAM専用*/
			frames Lightsource_frame;				//光源
			LightHandle Light;						//ライト
			/*サイト専用*/
			VECTOR_ref sight_vec;					//サイトが見ている場所
			frames sight_frame;						//サイト
			GraphHandle reticle;					//レティクル
			/*MV1 obj_col;*/
			/*マガジン専用*/
			std::array<MV1, 2> obj_ammo;			//マガジン用弾
		public:
			/*getter*/
			//frame
			const auto& Get_slide_frame(void) const noexcept		{ return this->slide_frame; }
			const auto& Get_sight_frame(void) const noexcept		{ return this->sight_frame.second; }
			//type
			const auto Get_mazzuletype(void) const noexcept			{ return (IsActive()) ? int(this->thisparts->mazzule_type) : -1; }
			const auto Get_stocktype(void) const noexcept			{ return (IsActive()) ? int(this->thisparts->stock_type) : -1; }
			const auto Get_lamtype(size_t sel_t) const noexcept		{ return (IsActive()) ? this->thisparts->select_lam[sel_t] : SELECTLAM_NUM; }
			//obj.frame
			const auto Get_mazzule_pos(void) const noexcept			{ return (IsActive()) ? this->obj.frame(this->Get_rail_frame(POINTS_MAZZULE_BASE).first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_magf_pos(void) const noexcept			{ return (IsActive()) ? this->obj.frame(this->mag_frame.first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_mag2f_pos(void) const noexcept			{ return (IsActive()) ? this->obj.frame(this->mag_frame.first + 1) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_source_pos(void) const noexcept			{ return (IsActive()) ? this->obj.frame(this->Lightsource_frame.first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_RIGHT_pos(size_t sel_t) const noexcept	{ return (IsActive()) ? this->obj.frame(this->RIGHT_frame[sel_t].first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_LEFT_pos(size_t sel_t) const noexcept	{ return (IsActive()) ? this->obj.frame(this->LEFT_frame[sel_t].first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_L_mag_pos(size_t sel_t) const noexcept	{ return (IsActive()) ? this->obj.frame(this->LEFT_mag_frame[sel_t].first) : VECTOR_ref::vget(0, 0, 0); }

			const auto Get_cate_pos(void) const noexcept			{ return (IsActive()) ? this->obj.frame(this->cate_frame.first) : VECTOR_ref::vget(0, 0, 0); }
			const auto Get_cate_vec(void) const noexcept			{ return (this->obj.frame(this->cate_frame.first + 1) - this->Get_cate_pos()).Norm(); }
			//
			const auto Get_sightvec(void) const noexcept			{ return (IsActive() && this->type == PARTS_SIGHT) ? this->sight_vec : VECTOR_ref::vget(0.f, 0.f, 0.f); }
			const auto Get_reticle_size(void) const noexcept		{ return (IsActive() && this->type == PARTS_SIGHT) ? float(y_r(this->thisparts->zoom_size / 2.f)) : 1.f; }
			/*コンストラクタ*/
			g_parts(void) noexcept {
				this->Light.Dispose();
			}

			void get_frame_(GUNPARTs& parts, EnumGunParts type_t) noexcept override {
				GPARTS_COMMON::get_frame_(parts, type_t);
				if (IsActive()) {
					this->mag_frame.first = -1;
					this->sight_frame.first = -1;
					this->cate_frame.first = -1;
					//this->sight_frame.second.clear();
					if (this->type == PARTS_MAGAZINE) {
						this->obj_ammo[0] = this->thisparts->ammo[0].get_model_full().Duplicate();
						this->obj_ammo[1] = this->thisparts->ammo[0].get_model_full().Duplicate();
					}
					for (int i = 0; i < this->obj.frame_num(); ++i) {
						std::string p = this->obj.frame_name(i);
						if (p == "sight" || p == "site") {
							this->sight_frame = { int(i),this->obj.frame(i) };
						}
						else if (p == "LEFT") {
							this->LEFT_frame[0] = { int(i),this->obj.frame(i) };
							this->LEFT_frame[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							this->LEFT_frame[2] = { int(i + 2),this->obj.GetFrameLocalMatrix(i + 2).pos() };
						}
						else if (p == "LEFT_mag") {
							this->LEFT_mag_frame[0] = this->thisparts->mod.getLEFT_mag_frame(0);
							this->LEFT_mag_frame[1] = this->thisparts->mod.getLEFT_mag_frame(1);
							this->LEFT_mag_frame[2] = this->thisparts->mod.getLEFT_mag_frame(2);
						}
						else if (p == "RIGHT") {
							this->RIGHT_frame[0] = { int(i),this->obj.frame(i) };
							this->RIGHT_frame[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							this->RIGHT_frame[2] = { int(i + 2),this->obj.GetFrameLocalMatrix(i + 2).pos() };
						}
						else if (p == "source") {
							this->Lightsource_frame = { int(i),this->obj.frame(i) };
						}
						else if (p == "mag_fall") {
							this->mag_frame = { i, this->obj.frame(i) };//マガジン
						}
						else if (p == "cate") {
							this->cate_frame = { i, this->obj.frame(i) };//排莢
						}
						else if (p == "slide") {
							this->slide_frame = { i, this->obj.frame(i) };//
						}
					}
					switch (this->type) {
					case PARTS_SIGHT:
					{
 						this->reticle = this->thisparts->reticle.Duplicate();
						this->obj.SetupCollInfo(1, 1, 1, -1, 1);//
						break;
					}
					case PARTS_LAM:
					{
						this->Light.Dispose();
						break;
					}
					default:
						break;
					}
				}
			}
			const auto Draw_reticle_UI(bool ads) noexcept {
				if (IsActive() && ads) {
					this->obj.RefreshCollInfo(-1, 1);
					VECTOR_ref sightpos = this->obj.frame(3);
					if (
						this->obj.CollCheck_Line(GetCameraPosition(), sightpos, -1, 1).HitFlag == TRUE
						) {
						VECTOR_ref tmp = ConvWorldPosToScreenPos(sightpos.get());
						if (tmp.z() >= 0.f && tmp.z() <= 1.f) {
							const auto siz = int(this->thisparts->reticle_size / 2.f);
							DrawLine(int(tmp.x()) - siz, int(tmp.y()) - siz, int(tmp.x()) + siz, int(tmp.y()) + siz, GetColor(255, 0, 0), 3);
							this->reticle.DrawExtendGraph(int(tmp.x()) - siz, int(tmp.y()) - siz, int(tmp.x()) + siz, int(tmp.y()) + siz, true);
							return this->thisparts->zoom;
						}
					}
				}
				return 1.f;
			}
			//パーツを付ける
			void attach_mini(performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t) noexcept {
				this->attach = true;
				this->get_frame_(partsptr, type_t);
				per_all += this->thisparts->per;
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t) noexcept {
				Detach(per_all);
				attach_mini(per_all, partsptr, type_t);
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t, GPARTS_COMMON* parents, size_t side) noexcept {
				Detach(per_all);
				this->attach = false;
				if (parents != nullptr) {
					if (parents->thisparts != nullptr) {
						for (auto& p : parents->thisparts->can_attach) {
							if ((p == partsptr.mod.get_name()) && (parents->Get_rail_frame(side).first > 0)) {
								attach_mini(per_all, partsptr, type_t);
								set_parent(parents, side);
								break;
							}
						}
					}
				}
			}
			//パーツを外す
			void Detach(performance& per_all) noexcept override {
				if (IsActive()) {
					GPARTS_COMMON::Detach(per_all);
					this->reticle.Dispose();
					this->Light.Dispose();
				}
			}
			//ライトの設定
			void Set_LightHandle(const VECTOR_ref& Vec) noexcept {
				switch (this->Get_lamtype(0)) {
				case SELECTLAM_LIGHT:
				{
					auto StartPos = this->Get_source_pos();
					if (this->Light.get() == -1) {
						this->Light = LightHandle::Create(StartPos, Vec, DX_PI_F / 16.0f, DX_PI_F / 24.0f, 25.0f, 0.1f, 0.35f, 0.0f);
					}
					this->Light.SetPos(StartPos, Vec);
					break;
				}
				default:
					this->Light.Dispose();
					break;
				}
			}
			//サイトの親の座標を探す
			void set_sight_vec(void) noexcept {
				if (IsActive()) {
					this->sight_vec = this->Get_attach_frame() + this->Get_sight_frame();
					search_parts(PARTS_BASE, &this->sight_vec, this);
				}
			}
			//サイトを乗せるかいなか
			bool Set_sight_at(const std::vector<GUNPARTs>& parts_data) noexcept {
				bool trm = false;
				if (IsActive()) {
					for (auto& can : this->thisparts->can_attach) {
						for (auto& tgt : parts_data) {
							if (can == tgt.mod.get_name()) { trm = true; }
							if (trm) { break; }
						}
						if (trm) { break; }
					}
				}
				return trm;
			}
		};
	private:
		//薬莢
		class CARTS {
			MV1 obj;
			bool Flag{ false };
			float cnt{ -1.f };
			bool se_use{ false };
			float rate{ 0.f };
			bool stay{ false };
			moves move;
		public:
			auto& Get_Flag() const noexcept { return Flag; }
			auto& Get_pos(void) noexcept { return move.pos; }

			void Set(void) noexcept {
				this->Dispose();
			}
			void Put_first(MV1&model, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				this->Dispose();
				this->Flag = true;
				this->se_use = false;
				this->obj = model.Duplicate();
				this->move.SetPos(pos_t);//排莢
				this->move.mat = mat_t;
				this->stay = true;
			}

			void Put_second(const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				if (this->Flag) {
					if (this->stay) {
						this->move.SetPos(pos_t);//排莢
						this->move.mat = mat_t;
					}
				}
			}
			void Put_stay(const VECTOR_ref& vec_t, const float rate_t = 1) noexcept {
				if (this->Flag) {
					if (this->stay) {
						this->cnt = 1.5f;
						this->rate = rate_t;
						this->move.vec = vec_t * this->rate;//排莢ベクトル
						this->stay = false;
					}
				}
			}

			void UpDate_pos(void) noexcept {
				if (this->Flag) {
					if (!this->stay) {
						this->cnt -= this->rate / FPS;
						this->move.Update_Physics(0.5f, this->rate);
					}
				}
			}
			void UpDate_fall(const MV1_COLL_RESULT_POLY& mapcol, const std::shared_ptr<PLAYERclass::Chara>& mine) noexcept {
				if (this->Flag) {
					if (this->stay) {
					}
					else {
						if (mapcol.HitFlag == TRUE) {
							VECTOR_ref map_nomal = mapcol.Normal;
							this->move.SetPos(VECTOR_ref(mapcol.HitPosition) + VECTOR_ref::vget(0, 0.008f, 0));
							this->move.vec += map_nomal * (map_nomal.dot(this->move.vec*-1.f)*1.25f);
							easing_set(&this->move.vec, VECTOR_ref::vget(0, 0, 0), 0.95f);
							if (!this->se_use) {
								mine->audio.case_down.play_3D(this->move.pos, 2.f);
								this->se_use = true;
							}
							this->move.mat *= MATRIX_ref::RotVec2(this->move.mat.yvec(), map_nomal);
						}
						else {
							this->move.mat *= MATRIX_ref::RotY(deg2rad(360 * 3 + GetRand(360 * 3 * 2))* this->rate / FPS);
						}
					}
					this->obj.SetMatrix(this->move.mat*MATRIX_ref::Mtrans(this->move.pos));
					if (this->cnt <= 0.f) {
						this->Flag = false;
					}
				}
			}
			void Draw(void) noexcept {
				if (this->Flag) {
					this->obj.DrawModel();
				}
			}
			void Dispose(void) noexcept {
				this->Flag = false;
				this->obj.Dispose();
			}
		};
		//所持弾数などの武器データ
		class MAG_STATUS {
			std::array<frames, 2> magazine_f;		//マガジンフレーム
		public:
			GUNPARTs* ptr_mag{ nullptr };
			MV1 obj_mag_;
			std::array<MV1, 2> obj_ammo;			//マガジン用弾
			std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム
			frames LEFT_mag_frame;
			size_t m_cnt{ 0 };
			size_t cap{ 30 };

			void Draw(void) const noexcept {
				obj_mag_.DrawModel();
				if (m_cnt >= 1) {
					obj_ammo[0].DrawModel();
				}
				if (m_cnt >= 2) {
					obj_ammo[1].DrawModel();
				}
			}

			auto& getmagazine_f(int i) const noexcept { return magazine_f[i]; }
			const auto Get_L_mag_pos(void) noexcept {
				return this->obj_mag_.frame(this->LEFT_mag_frame.first);
			}

			void Set(GUNPARTs* ptr_mag_t, size_t m_cnt_t) {
				if (ptr_mag_t != nullptr) {
					this->m_cnt = m_cnt_t;
					this->ptr_mag = ptr_mag_t;
					this->cap = this->ptr_mag->mag_cnt;
					this->obj_mag_ = this->ptr_mag->mod.get_model().Duplicate();
					this->obj_ammo[0] = this->ptr_mag->ammo[0].get_model_full().Duplicate();
					this->obj_ammo[1] = this->ptr_mag->ammo[0].get_model_full().Duplicate();
					this->magazine_ammo_f[0] = this->ptr_mag->mod.getmagazine_ammo_f(0);
					this->magazine_ammo_f[1] = this->ptr_mag->mod.getmagazine_ammo_f(1);
					this->LEFT_mag_frame = this->ptr_mag->mod.getLEFT_mag_frame(0);
					this->magazine_f[0] = this->ptr_mag->mod.getmagazine_f(0);
					this->magazine_f[1] = this->ptr_mag->mod.getmagazine_f(1);
				}
			}
		};
		class GUN_STATUS {
		private:
			bool in_chamber{ false };			//チャンバー内
			size_t ammo_cnt{ 0 };				//チャンバー内
			uint8_t select{ 0 };					//セレクター
			std::vector<MAG_STATUS> magazine_in;	//マガジン内データ
			uint8_t selecter{ 0 };					//セレクター
		private:
			size_t get_magazine_in_not_full(void) noexcept {
				auto result = std::find_if(this->magazine_in.begin() + 1, this->magazine_in.end(), [](MAG_STATUS& m) { return (m.m_cnt != m.cap); });
				return (result != this->magazine_in.end()) ? result - this->magazine_in.begin() : 0;
			}
		public:
			const auto& get_selecter() { return selecter; }
			auto& get_magazine_in(void) noexcept { return magazine_in; }
			//チャンバーに弾があるか否か
			bool not_chamber_EMPTY(void) const noexcept { return in_chamber; }
			//
			GUN_STATUS() {
				Reset();
			}
			GUN_STATUS(const GUN_STATUS &) {
			}
			~GUN_STATUS() {
			}
			//
			void get_magazine_in_pop_front(void) noexcept { magazine_in.erase(magazine_in.begin()); }
			void get_magazine_in_pop_back(void) noexcept { magazine_in.pop_back(); }
			//
			void selector_set(const size_t& sel) { selecter = uint8_t(sel); }
			//マガジンを複数持っているか
			bool hav_mags(void) const noexcept { return magazine_in.size() > 1; }
			//マガジンを1つ以上持っているか
			bool hav_mag(void) const noexcept { return magazine_in.size() > 0; }
			//リセット
			void Reset(void) noexcept {
				this->magazine_in.clear();
				in_chamber = false;
			}
			//マガジンを1つ追加(装填無し)
			void magazine_plus(Items* magazine_item) noexcept {
				this->magazine_in.resize(this->magazine_in.size() + 1);
				this->magazine_in.back().Set(magazine_item->get_ptr_mag(), magazine_item->get_magazine().mag_cnt);
			}
			//マガジンを1つ追加
			void magazine_pushback(GUNPARTs* magazine_parts) noexcept {
				this->magazine_in.resize(this->magazine_in.size() + 1);
				this->magazine_in.back().Set(magazine_parts, magazine_parts->mag_cnt);

				if (this->magazine_in.size() == 1) {
					this->magazine_in.back().m_cnt--;
					in_chamber = true;
				}
			}
			//撃った時の弾数管理
			void magazine_shot(const bool reload_f) noexcept {
				if (reload_f) {
					//チャンバーを空にする
					this->in_chamber = false;
				}
				else {
					//マガジンを持っているなら
					if (this->hav_mag()) {
						//チャンバーに1発残っているなら
						if (this->magazine_in.front().m_cnt > 0) {
							//マガジンからチャンバーのカウントを抜く
							this->magazine_in.front().m_cnt--;
						}
						else {
							this->in_chamber = false;
						}
					}
					else {
						this->in_chamber = false;
					}
				}
			}
			//チャンバーに装填
			void chamber_in(void) noexcept {
				if (!this->in_chamber) {
					this->magazine_in.front().m_cnt--;
					this->in_chamber = true;
				}
			}
			//マグ順ソート
			void sort_magazine(void) noexcept {
				std::sort(this->magazine_in.begin() + 1, this->magazine_in.end(), [](MAG_STATUS& a, MAG_STATUS& b) { return a.m_cnt > b.m_cnt; });
			}
			//弾抜き、装填
			void load_magazine(void) noexcept {
				size_t siz = this->get_magazine_in_not_full();
				auto be_ = std::clamp<size_t>(this->magazine_in.back().m_cnt, 0, magazine_in[siz].cap - this->magazine_in[siz].m_cnt);
				this->magazine_in.back().m_cnt -= be_;
				this->magazine_in[siz].m_cnt += be_;
			}
			//
		};
		//体のフレーム情報
		class frame_body {
		public:
			//頭
			frames head_f;
			//左目
			frames LEFTeye_f;
			frames RIGHTeye_f;
			//胴体
			frames bodyg_f;
			frames bodyc_f;
			frames bodyb_f;
			frames body_f;
			//右手座標系
			frames RIGHThand2_f;
			frames RIGHThand_f;
			frames RIGHTarm2_f;
			frames RIGHTarm1_f;
			//左手座標系
			frames LEFThand2_f;
			frames LEFThand_f;
			frames LEFTarm2_f;
			frames LEFTarm1_f;

			//右手座標系
			frames RIGHTreg2_f;
			frames RIGHTreg_f;
			frames RIGHTfoot2_f;
			frames RIGHTfoot1_f;
			//左手座標系
			frames LEFTreg2_f;
			frames LEFTreg_f;
			frames LEFTfoot2_f;
			frames LEFTfoot1_f;
			//
			std::array<frames, 3> LEFTBodyFrame;		//左手座標(マガジン保持時)
		public:
			//
			void get_frame(MV1& obj_) noexcept {
				for (int i = 0; i < int(obj_.frame_num()); ++i) {
					std::string p = obj_.frame_name(i);
					if (p == std::string("グルーブ")) {
						this->bodyg_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("下半身")) {
						this->bodyc_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->LEFTfoot1_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひざ") != std::string::npos) {
						this->LEFTfoot2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->LEFTreg_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首先") != std::string::npos) {
						this->LEFTreg2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->RIGHTfoot1_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひざ") != std::string::npos) {
						this->RIGHTfoot2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->RIGHTreg_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首先") != std::string::npos) {
						this->RIGHTreg2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
						this->bodyb_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身2") != std::string::npos) {
						this->body_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
						this->head_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
						//head_hight = obj_.frame(this->head_f.first).y();
					}
					else if (p.find("右目先") != std::string::npos) {
						this->RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左目先") != std::string::npos) {
						this->LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひじ") != std::string::npos) {
						this->RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("右手首")) {
						this->RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("右手先") || p == std::string("右手首先")) {
						this->RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひじ") != std::string::npos) {
						this->LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("左手首")) {
						this->LEFThand_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("左手先") || p == std::string("左手首先")) {
						this->LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VECTOR_ref::vget(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					if (p == std::string("LEFT_mag")) {//LEFT
						this->LEFTBodyFrame[0] = { int(i),obj_.frame(i) };
						this->LEFTBodyFrame[1] = { int(i + 1),obj_.GetFrameLocalMatrix(i + 1).pos() };
						this->LEFTBodyFrame[2] = { int(i + 2),obj_.GetFrameLocalMatrix(i + 2).pos() };
					}
				}
			}
			//
			void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) noexcept {
				tgt->SetMatrix(mine.GetMatrix());
				//
				tgt->SetFrameLocalMatrix(frame_tgt_.head_f.first, mine.GetFrameLocalMatrix(this->head_f.first));
				//
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTeye_f.first, mine.GetFrameLocalMatrix(this->LEFTeye_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTeye_f.first, mine.GetFrameLocalMatrix(this->RIGHTeye_f.first));
				//
				tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.first, mine.GetFrameLocalMatrix(this->bodyg_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.first, mine.GetFrameLocalMatrix(this->bodyc_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.first, mine.GetFrameLocalMatrix(this->bodyb_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.body_f.first, mine.GetFrameLocalMatrix(this->body_f.first));
				//右手座標系
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.first, mine.GetFrameLocalMatrix(this->RIGHThand2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.first, mine.GetFrameLocalMatrix(this->RIGHThand_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.first, mine.GetFrameLocalMatrix(this->RIGHTarm2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.first, mine.GetFrameLocalMatrix(this->RIGHTarm1_f.first));
				//左手座標系
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.first, mine.GetFrameLocalMatrix(this->LEFThand2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.first, mine.GetFrameLocalMatrix(this->LEFThand_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.first, mine.GetFrameLocalMatrix(this->LEFTarm2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.first, mine.GetFrameLocalMatrix(this->LEFTarm1_f.first));
				//右手座標系
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.first, mine.GetFrameLocalMatrix(this->RIGHTreg2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.first, mine.GetFrameLocalMatrix(this->RIGHTreg_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.first, mine.GetFrameLocalMatrix(this->RIGHTfoot2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.first, mine.GetFrameLocalMatrix(this->RIGHTfoot1_f.first));
				//左手座標系
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.first, mine.GetFrameLocalMatrix(this->LEFTreg2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.first, mine.GetFrameLocalMatrix(this->LEFTreg_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.first, mine.GetFrameLocalMatrix(this->LEFTfoot2_f.first));
				tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.first, mine.GetFrameLocalMatrix(this->LEFTfoot1_f.first));
				for (int i = 0; i < tgt->get_anime().size(); ++i) {
					tgt->get_anime(i).per = mine.get_anime(i).per;
					tgt->get_anime(i).time = mine.get_anime(i).time;
				}
			}
			//
		};
		//操作全般
		class opes {
		public:
			//
			bool wkey{ false };
			bool skey{ false };
			bool akey{ false };
			bool dkey{ false };
			bool qkey{ false };
			bool ekey{ false };
			//
			bool running{ false };		//走るか否か
			bool jamp{ false };			//ジャンプ
			bool shoot{ false };			//射撃
			bool reload{ false };		//リロード開始
			bool have_mag{ false };		//マガジンを持つか
			bool have_item{ false };		//アイテム取得
			bool sort_magazine{ false };	//マガジンソート
			bool view_gun{ false };	//マガジンソート
			bool drop_{ false };			//医療品落とす
			bool throw_gre{ false };		//グレ投げ
			bool aim{ false };			//エイム
			bool TPS{ false };			//TPS
			bool squat{ false };			//しゃがむ
			bool select{ false };		//セレクター

			void ReSet_(void) noexcept {
				this->wkey = false;
				this->skey = false;
				this->akey = false;
				this->dkey = false;
				this->shoot = false;
				this->running = false;
				this->qkey = false;
				this->ekey = false;
				this->aim = false;
				this->TPS = false;
				this->reload = false;
				this->have_item = false;
				this->sort_magazine = false;
				this->view_gun = false;
				this->drop_ = false;
				this->throw_gre = false;
				this->select = false;
				this->have_mag = false;
				this->jamp = false;
				this->squat = false;
			}
			void rule_(void) noexcept {
				if ((this->skey) || (this->aim) || (!this->wkey) || (!this->wkey && !this->skey && !this->akey && !this->dkey)) {
					this->running = false;
				}
				if (this->running) {
					this->squat = false;
					this->skey = false;
					this->qkey = false;
					this->ekey = false;
				}
			}
		};
		//VR用の関節
		class Pos_VRJoint {
		public:
			moves move;
			MATRIX_ref mat_notlean;
#ifdef _USE_OPENVR_
		public:
			moves move_start;
		private:
			std::shared_ptr<DXDraw> DrawPts{ nullptr };
			bool oldv_2{ false };
			bool oldv_2_2{ false };
		public:
			auto get_pos_noY() const noexcept { return VECTOR_ref::vget(this->move.pos.x(), 0.f, this->move.pos.z()); }
			auto get_pos_old_noY() const noexcept { return VECTOR_ref::vget(this->move.repos.x(), 0.f, this->move.repos.z()); }
			MATRIX_ref get_mat_XZ() const noexcept { return MATRIX_ref::Axis1(this->move.mat.xvec()*-1.f, this->move.mat.yvec(), this->move.mat.zvec()*-1.f); }

			void Set(std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
				DrawPts = DrawPts_t;
				oldv_2 = true;
				oldv_2_2 = true;
			}
			void UpDate_none(const char& device_num) noexcept {
				DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);
				this->move.mat = get_mat_XZ();
			}
			void UpDate_pos(const char& device_num, const bool flag_start_loop) noexcept {
				auto ptr_ = &(*DrawPts->get_device())[device_num];
				DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);
				if (flag_start_loop || (ptr_->turn && ptr_->now) != this->oldv_2) {
					this->move_start.SetPos(get_pos_noY());
				}
				this->oldv_2 = ptr_->turn && ptr_->now;
			}
			void UpDate_mat(const char& device_num, const bool flag_start_loop) noexcept {
				auto ptr_ = &(*DrawPts->get_device())[device_num];
				DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);
				this->move.mat = get_mat_XZ();
				if ((flag_start_loop || (ptr_->turn && ptr_->now) != this->oldv_2) && this->oldv_2_2) {
					this->move_start.mat = this->move.mat;
				}
				this->oldv_2 = ptr_->turn && ptr_->now;
			}
#endif // _USE_OPENVR_
		};
		//スコア
		class score_s {
			float death_timer{ 0.f };
		public:
			float kill_timer{ 0.f };

			int kill_cnt{ 0 };
			int death_cnt{ 0 };
			size_t kill_id{ 0 };
			size_t death_id{ 0 };
			bool kill_f{ false };
			bool death_f{ false };

			int score{ 0 };
			int kill_streak{ 0 };
			//
			void reset(bool allreset) {
				this->kill_f = false;
				this->death_f = false;
				this->kill_id = 0;
				this->kill_streak = 0;
				this->kill_timer = 0.f;
				this->death_id = 0;
				if (allreset) {
					this->score = 0;
					this->kill_cnt = 0;
					this->death_cnt = 0;
				}
			}
			void UpDate(void) noexcept {
				if (this->death_f) {
					this->death_timer -= 1.f / FPS;
					if (this->death_timer <= 0.f) {
						this->death_timer = 0.f;
						this->death_f = false;
					}
				}

				if (this->kill_f) {
					this->kill_timer -= 1.f / FPS;
					if (this->kill_timer <= 0.f) {
						this->kill_timer = 0.f;
						this->kill_streak = 0;
						this->kill_f = false;
					}
				}
			}
			void set_kill(size_t deathid, int addpoint) {
				this->kill_f = true;
				this->kill_id = deathid;
				this->kill_timer = 7.f;
				++this->kill_cnt;
				//
				if (this->kill_timer != 0.f) {
					++this->kill_streak;
					this->score += std::clamp(25 - this->kill_streak * 5, 0, 25);
				}
				this->score += addpoint;
			}
			void set_death(size_t killid) {
				this->death_f = true;
				this->death_id = killid;
				this->death_timer = 3.f;
				++this->death_cnt;
			}
			bool start_death(void) const noexcept { return death_timer == 3.f; }
		};
		//腕アニメ関連
		class anime_arm {
		public:
			float pers{ 0.f };
			float total{ 1.f };
		};
		class anime_vector {
		public:
			std::array<VECTOR_ref, 3> vec;		//座標
		};
		anime_vector* prev_ani_lefthand{ nullptr };
		anime_vector* next_ani_lefthand{ nullptr };
		anime_arm* now_timeline{ nullptr };
		std::vector<anime_arm> ani_timeline;
		std::vector<anime_vector> ani_lefthand;
		VECTOR_ref vec_gunani{ VECTOR_ref::vget(0.f, 0.f, 1.f) };
		VECTOR_ref pos_gunani;
		float zrad_gunani{ 0.f };
		bool have_magazine{ true };						//マガジンを持つか
		bool throw_grenades{ false };					//グレネードを投げるか
		bool throw_grenade_old{ false };				//グレネードを投げるか()
		//AI用
		class AI {
		public:
			float ai_time_a{ 0.f };
			float ai_time_d{ 0.f };
			float ai_time_q{ 0.f };
			float ai_time_e{ 0.f };
			float ai_time_shoot{ 0.f };
			float ai_time_walk{ 0.f };
			bool ai_reload{ false };
			int ai_phase{ 0 };
			std::array<int, 6> wayp_pre{ 0 };

			void Spawn(int now) {
				this->ai_phase = 0;
				this->ai_time_shoot = 0.f;
				this->ai_time_a = 0.f;
				this->ai_time_d = 0.f;
				this->ai_time_e = 0.f;
				this->fill_wayp_pre(now);
			}
			void fill_wayp_pre(int now) {
				for (auto& w : this->wayp_pre) { w = now; }
			}
			void set_wayp_pre(int now) {
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[i - 1]; }
				this->wayp_pre[0] = now;
			}
			void set_wayp_return() {
				auto ppp = this->wayp_pre[1];
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[0]; }
				this->wayp_pre[0] = ppp;
			}
		};
	private:
		MV1 obj_lag;									//ラグドール
		moves spawn;									//プレイヤースポーン座標
		VECTOR_ref add_vec_buf, add_vec_real;			//移動ベクトルバッファ
		VECTOR_ref pos_tt;								/*プレイヤー位置*/
		std::array<EffectS, effects> effcs;				/*エフェクト*/
		std::array<EffectS, 12> effcs_gndhit;			/*エフェクト*/
		size_t use_effcsgndhit{ 0 };					/*エフェクト*/
		size_t use_bullet{ 0 };							/*確保する弾*/
		std::array<BULLETS, max_bullet> bullet;			/*確保する弾*/
		std::array<CARTS, max_bullet> cart;				/*確保する弾*/
		std::vector<GUN_STATUS> gun_stat;				/*所持弾数などの武器データ*/
		Audios audio;									/*音声*/
		MV1::ani* gunanime_shot{ nullptr };				//銃anime
		MV1::ani* gunanime_first{ nullptr };			//銃anime
		MV1::ani* gunanime_shot_last{ nullptr };		//銃anime
		MV1::ani* gunanime_trigger{ nullptr };			//銃anime
		MV1::ani* gunanime_magcatch{ nullptr };			//銃anime
		std::vector<MV1::ani*>gunanime_sel{ nullptr };	//銃anime
		MV1::ani* anime_walk{ nullptr };				//脚anime
		MV1::ani* anime_swalk{ nullptr };				//脚anime
		MV1::ani* anime_run{ nullptr };					//脚anime
		MV1::ani* anime_sit{ nullptr };					//脚ポーズ
		MV1::ani* anime_wake{ nullptr };				//脚ポーズ
		MV1::ani* anime_jump{ nullptr };				//脚ポーズ
		MV1::ani* anime_hand_nomal{ nullptr };			//基本手ポーズ
		MV1::ani* anime_hand_trigger{ nullptr };		//引き金ポーズ
		MV1::ani* anime_hand_trigger_pull{ nullptr };	//引き金引くポーズ
		float reload_cnt{ 0.f };						//リロード開始までのカウント
		switchs trigger;								//トリガー
		bool nearhit{ false };							//
		MATRIX_ref mat_body;							//位置
		float xrad_p{ 0.f };							//マウスエイム用変数確保
		bool sort_ing{ false };							//整頓
		bool view_ing{ false };							//眺める
		size_t view_ings{ 0 };							//
		size_t reload_ings{ 0 };						//
		size_t throw_gre_ings{ 0 };						//
		VECTOR_ref gunpos;								//マウスエイム用銃座標
		opes key_;										//入力関連
		AI cpu_do;										//AI用
		moves mag_m;									//マガジン座標系
		moves gun_m;									//銃座標
		VECTOR_ref blur_vec, blur_vec_res;				//
		VECTOR_ref recoil_vec, recoil_vec_res;			//
		float rad_gun{ 0.f };							//
		Pos_VRJoint HMD;								//頭部座標系
		float add_ypos{ 0.f };							//垂直加速度
		float body_xrad{ 0.f };							//胴体角度
		float body_yrad{ 0.f };							//胴体角度
		float body_zrad{ 0.f };							//胴体角度
		float body_xtrun{ 0.f };						//胴体角度
		float body_ztrun{ 0.f };						//胴体角度
		frame_body frame_s;								//フレーム
		frame_body lagframe_;							//フレーム
		frame_body colframe_;							//フレーム
		Pos_VRJoint RIGHTHAND;							//右手座標系
		Pos_VRJoint LEFTHAND;							//左手座標系
		Pos_VRJoint RIGHTREG;							//右脚座標系
		Pos_VRJoint LEFTREG;							//左脚座標系
		Pos_VRJoint WAIST;								//腰座標
		float distance_to_cam{ -1.f };					//カメラとの距離
		float ratio_move{ 0.f };						//移動の度合い(アニメ)
		performance per_all;							//性能
		size_t canget_id{ 0 };							//取得可能アイテムID
		bool canget_magitem_f{ false };					//取得可能アイテム(マガジン)
		std::string canget_mag;							//取得可能アイテム(マガジン)
		bool canget_meditem_f{ false };					//取得可能アイテム(衣料品)
		std::string canget_med;							//取得可能アイテム(衣料品)
		size_t select_sight{ 0 };						//使用するサイト
		/*フラグ*/
		bool LEFT_hand{ false };						//左手を添えるフラグ
		bool flag_gun{ false };							//射撃フラグ
		bool flag_canlook_player{ true };				//プレイヤーが視界内か否かのフラグ
		bool flag_calc_body{ true };					//身体の物理演算フラグ
		bool flag_start_loop{ true };					//演算開始フラグ
		bool flag_calc_lag{ true };						//ラグドールの物理演算フラグ
		/*モデル*/
		g_parts base;
		g_parts magazine;
		g_parts dustcover;
		g_parts stock;
		g_parts foregrip;
		g_parts lam;
		g_parts mazzule;
		g_parts grip;
		g_parts uperhandguard;
		g_parts underhandguard;
		g_parts mount_base;
		g_parts mount_;
		std::vector<g_parts> sight_;
		/*テスト用*/
		//float zpos_cart{ 0.f };						//薬莢のzpos
	public:
		bool gre_eff{ false };
		/*所持弾数などの武器データ*/
		GUN_STATUS* gun_stat_now{ nullptr };
		//スコア
		score_s scores;
		bool sort_f{ false };
		bool reloadf{ false };
	public:
		//constにできないgetter
		auto& get_key_(void) noexcept { return key_; }																										//key
		g_parts* get_parts(size_t type_sel, int mount_t = 0) noexcept {
			switch (type_sel) {
			case PARTS_BASE:
				return &this->base;
			case PARTS_MAGAZINE:
				return &this->magazine;
			case PARTS_GRIP:
				return &this->grip;
			case PARTS_UPER_HGUARD:
				return &this->uperhandguard;
			case PARTS_UNDER_HGUARD:
				return &this->underhandguard;
			case PARTS_MAZZULE:
				return &this->mazzule;
			case PARTS_DUSTCOVER:
				return &this->dustcover;
			case PARTS_STOCK:
				return &this->stock;
			case PARTS_LAM:
				return &this->lam;
			case PARTS_FOREGRIP:
				return &this->foregrip;
			case PARTS_MOUNT_BASE:
				return &this->mount_base;
			case PARTS_MOUNT:
				return &this->mount_;
			case PARTS_SIGHT:
				return &this->sight_[mount_t];
			default:
				return nullptr;
			}
		}
		//getter
		const auto BodyFrame(const int& p1) const noexcept { return obj_body.frame(p1); }
		const auto BodyFrameMatrix(const int& id) const noexcept { return obj_body.GetFrameLocalWorldMatrix(id); }
		const auto Get_reticle_size(void) const noexcept {
			if (select_sight != SIZE_MAX) {
				return this->sight_[select_sight].Get_reticle_size();
			}
			else {
				return 1.f;
			}
		}
		const auto get_maz(void) const noexcept { if (mazzule.Get_mazzuletype() > 0) { return mazzule.Get_mazzule_pos(); } else { return base.Get_mazzule_pos(); } }
		const bool get_alive(void) const noexcept { return this->HP != 0; }																					//生きているか
		const VECTOR_ref get_pos(bool no_y = false) const noexcept {
#ifdef _USE_OPENVR_
			if (DrawPts->use_vr) {
				if (!no_y) {
					return this->pos_tt + this->HMD.move.pos - this->HMD.move_start.pos;
				}
				else {
					return this->pos_tt + this->HMD.get_pos_noY() - this->HMD.move_start.pos;
				}
			}
#endif // _USE_OPENVR_
			if (!no_y) { return this->pos_tt + this->HMD.move.pos; }
			else { return this->pos_tt; }
		}		//pos
		const auto ads_on(void) const noexcept { return this->key_.aim && !this->view_ing; }					//ADS中
		const auto get_jamping(void) const noexcept { return this->add_ypos != 0.f; }							//ジャンプ中
		const auto get_head_pos(void) const noexcept { return this->BodyFrame(this->frame_s.head_f.first); }	//頭部座標
		const auto get_pseed_per(void) const noexcept { return add_vec_real.size() / speed; }					//移動速度のパーセンテージ
		//参照getter
		const auto& get_now_selector(void) const noexcept { return this->base.thisparts->select[this->gun_stat_now->get_selecter()]; }	//
		const auto& get_audio() const noexcept { return this->audio; }																	//audio(メニューのみ)
		const auto& get_gunanime_trigger(void) const noexcept { return gunanime_trigger; }												//トリガーアニメーション取り出し(メニューのみ)
		const auto& get_mat_gun(void) const noexcept { return gun_m.mat; }																//
		const auto& get_body_yrad(void) const noexcept { return this->body_yrad; }														//ミニマップ用
		const auto& get_per_all(void) const noexcept { return per_all; }																//性能(UI表示用)
		const auto& getmagazine_push(void) const noexcept { return this->key_.have_item; }												//item関連
		const auto& get_pos_LEFTHAND(void) const noexcept { return LEFTHAND.move.pos; }													//item関連
		const auto& get_mat_LEFTHAND(void) const noexcept { return LEFTHAND.move.mat; }													//item関連
		const auto& get_canget_mag_f(void) const noexcept { return canget_magitem_f; }													//item関連
		const auto& get_canget_med_f(void) const noexcept { return canget_meditem_f; }													//item関連
		const auto& get_canget_mag_s(void) const noexcept { return canget_mag; }														//item関連
		const auto& get_canget_med(void) const noexcept { return canget_med; }															//item関連
	public:
		//
		void set_basepos(const VECTOR_ref& pos_t) {
			this->pos_tt = pos_t;
		}
		void Set_eff(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) {
			this->effcs[ef_].Set(pos_t, nomal_t, scale);
		}
		void Set_gndhit(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t,float scale) {
			this->effcs_gndhit[this->use_effcsgndhit].Set(pos_t, nomal_t, scale);
			++this->use_effcsgndhit %= this->effcs_gndhit.size();
		}
		//
		void calc_gredamage(const std::shared_ptr<PLAYERclass::Chara>& mine, const std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara) {
			if (this->get_alive()) {
				float scale = (this->move.pos - this->get_head_pos()).size();
				if (scale < 10.f) {
					if (!(MAPPTs->map_col_line(this->move.pos, this->get_head_pos()).HitFlag == TRUE)) {
						int damage = int(150.f * (10.f - scale) / 10.f);
						damage = std::clamp(damage, 0, 100);
						auto old = this->HP;
						this->HP = std::clamp(this->HP - damage, 0, this->HP_full);
						this->HP_parts[0] = std::clamp(this->HP_parts[0] - damage, 0, this->HP_full);
						this->got_damage = old - this->HP;
						this->got_damage_color = GetColor(255, 255, 255);

						if (float(this->HP) / float(this->HP_full) <= 0.66) {
							this->got_damage_color = GetColor(255, 255, 0);
						}
						if (float(this->HP) / float(this->HP_full) <= 0.33) {
							this->got_damage_color = GetColor(255, 128, 0);
						}
						if ((damage) != this->got_damage) {
							this->got_damage_color = GetColor(255, 0, 0);
						}
						this->got_damage_x = -255 + GetRand(255 * 2);
						this->got_damage_f = 1.f;
						{
							float x_1 = sinf(this->body_yrad);
							float y_1 = cosf(this->body_yrad);
							auto vc = (mine->get_pos() - this->get_pos()).Norm();
							this->got_damage_.resize(this->got_damage_.size() + 1);
							this->got_damage_.back().alpfa = 1.f;
							this->got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
						}
						if (!this->get_alive()) {
							auto me_ = shared_from_this();
							mine->scores.set_kill(&me_ - &chara.front(), 70);
							this->scores.set_death(&mine - &chara.front());
							this->get_audio().voice_death.play_3D(this->get_pos(), 10.f);
						}
						else {
							this->get_audio().voice_damage.play_3D(this->get_pos(), 10.f);
						}
						this->gre_eff = true;
						return;
					}
				}
			}
			return;
		}

		//サイトを画面に表示
		const float DrawReticle_UI(void) noexcept {
			if (select_sight != SIZE_MAX) {
				return this->sight_[select_sight].Draw_reticle_UI(this->ads_on());
			}
			else {
				return 1.f;
			}
		}
		//flag_gunをOFFからONにする
		const auto set_flag_gun(void) noexcept {
			if (!this->flag_gun) {
				this->flag_gun = true;
				return true;
			}
			return false;
		}
		//item
		void reset_canget_item(void) noexcept {
			canget_magitem_f = false;
			canget_meditem_f = false;
			return;
		}
		void addf_canget_magitem(bool zz) noexcept {
			canget_magitem_f |= zz;
			return;
		}
		void addf_canget_meditem(bool zz) noexcept {
			canget_meditem_f |= zz;
			return;
		}
		void set_canget_mag(size_t id_t, const std::string &itemname) noexcept {
			canget_id = id_t;
			canget_mag = itemname;
		}
		void set_canget_med(size_t id_t, const std::string &itemname) noexcept {
			canget_id = id_t;
			canget_med = itemname;
		}
	private:
		void BodyFrameLocalMatrix(const frames& id, const MATRIX_ref& mat_t = MGetIdent()) const noexcept { obj_body.SetFrameLocalMatrix(id.first, mat_t*MATRIX_ref::Mtrans(id.second)); }
		//右腕座標(固定)
		const auto RIGHT_pos_gun(size_t sel_t) noexcept { return this->base.Get_RIGHT_pos(sel_t); }
		//左腕座標
		const auto LEFT_pos_gun(size_t sel_t) noexcept {
			auto ans = this->underhandguard.Get_LEFT_pos(sel_t);
			auto f_ = this->foregrip.Get_LEFT_pos(sel_t);
			if (f_ != VECTOR_ref::vget(0, 0, 0)) { ans = f_; }
			return ans;
		}
	public:
		//視点取得
		void set_HMDpos(void) noexcept {
			this->HMD.move.pos = (this->BodyFrame(this->frame_s.RIGHTeye_f.first) + (this->BodyFrame(this->frame_s.LEFTeye_f.first) - this->BodyFrame(this->frame_s.RIGHTeye_f.first))*0.5f) - this->pos_tt;
		}
		const auto& GetHMDmat() { return HMD.move.mat; }
	private:
		//腕アニメで使う座標の決定
		void Set_GUN_pos_Anim(void) noexcept {
			for (int sel_t = 0; sel_t < 3; ++sel_t) {
				ani_lefthand[0].vec[sel_t] = this->LEFT_pos_gun(sel_t);
				ani_lefthand[1].vec[sel_t] = this->base.Get_L_mag_pos(sel_t);
				ani_lefthand[2].vec[sel_t] = this->BodyFrame(this->frame_s.LEFTBodyFrame[sel_t].first);
			}
		}
		//腕アニメの指定関数
		auto Func_Set_LEFT_pos_Anim(anime_arm*anim, anime_vector*next_ani, const VECTOR_ref& vec_gun, const VECTOR_ref& pos_gun_t, float zrad_gun) noexcept {
			now_timeline = anim;
			easing_set(&vec_gunani, vec_gun, 0.8f);
			easing_set(&pos_gunani, pos_gun_t, 0.5f);
			easing_set(&zrad_gunani, zrad_gun, 0.9f);
			//更新
			if (now_timeline->pers < now_timeline->total) {
				next_ani_lefthand = next_ani;
				now_timeline->pers += 1.f / FPS;
			}
			if (now_timeline->pers >= now_timeline->total) {
				prev_ani_lefthand = next_ani_lefthand;
				now_timeline->pers = now_timeline->total;
			}
			//リセット処理
			for (auto& tt : this->ani_timeline) {
				if (now_timeline != &tt) {
					tt.pers = 0.f;
				}
			}
			return now_timeline->pers == now_timeline->total;//終了フラグ
		}
	private:
		//リロード中か否か
		const auto isReload(void) const noexcept { return this->reloadf && this->gun_stat_now->hav_mag(); }//リロード中
		//腕アニメを参照
		const auto LEFT_pos_Anim(size_t sel_t) noexcept {
			if (now_timeline != nullptr) {
				if (now_timeline->pers < now_timeline->total) {
					auto pers = now_timeline->pers / now_timeline->total;
					return prev_ani_lefthand->vec[sel_t] * (1.f - pers) + next_ani_lefthand->vec[sel_t] * pers;
				}
				else {
					return next_ani_lefthand->vec[sel_t];
				}
			}
			else {
				return next_ani_lefthand->vec[sel_t];
			}
		}
		//腕アニメの決定
		void Set_LEFT_pos_Anim(void) noexcept {
			if (!isReload()) {
				//リロード初期化
				this->have_magazine = false;
				//
				this->reload_ings = 0;
			}
			//
			this->ani_timeline[0].total = 0.4f;//構える
			this->ani_timeline[1].total = 0.4f;//走る
			this->ani_timeline[2].total = 0.85f;//マガジンリリース
			this->ani_timeline[3].total = this->base.thisparts->reload_time - this->ani_timeline[2].total - 0.2f;//マガジン装着
			this->ani_timeline[4].total = 0.2f;//マガジン装着
			this->ani_timeline[5].total = 0.2f;//マグ整頓
			this->ani_timeline[6].total = 0.2f;//初段装填
			this->ani_timeline[7].total = 0.4f;//眺め1
			this->ani_timeline[8].total = 1.6f;//眺め2
			this->ani_timeline[9].total = 0.4f;//眺め3
			this->ani_timeline[10].total = 1.6f;//眺め4

			this->ani_timeline[11].total = 0.4f;//左腕が落ちる
			this->ani_timeline[12].total = 0.8f;//グレ投擲
//
			{
				if (this->key_.running && !isReload() && !this->sort_ing && !this->view_ing) {
					//走る
					VECTOR_ref vec_tgt = VECTOR_ref::vget(-0.75f, 0.15f, 0.35f);
					VECTOR_ref pos_tgt = VECTOR_ref::vget(0.05f + (sin(DX_PI_F*3.f*(this->anime_run->time / this->anime_run->alltime))*this->anime_run->per*0.04f), -0.04f + (sin(DX_PI_F*3.f*(this->anime_run->time / this->anime_run->alltime))*this->anime_run->per*0.012f), -0.1f);
					Func_Set_LEFT_pos_Anim(&this->ani_timeline[1], &this->ani_lefthand[0], vec_tgt, pos_tgt, -80.f);
					return;
				}
				else {
					if (this->isReload()) {
						switch (this->reload_ings) {
						case 0:
						{
							//リロード開始
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[2], &this->ani_lefthand[2], VECTOR_ref::vget(0.35f, -0.25f, 0.9f), VECTOR_ref::vget(0, 0.1f, -0.025f), 15.f)) {
								this->reload_ings++;
								this->have_magazine = true;
							}
							return;
						}
						case 1:
						{
							//マガジン装着
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[3], &this->ani_lefthand[1], VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 0.f)) {
								this->reload_ings++;
							}
							return;
						}
						case 2:
						{
							//マガジン装着後
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[4], &this->ani_lefthand[1], VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 0.f)) {
								this->reload_ings = 0;
							}
							return;
						}
						default:
							break;
						}
					}
					//マグ整頓
					if (this->sort_ing) {
						Func_Set_LEFT_pos_Anim(&this->ani_timeline[5], &this->ani_lefthand[2], VECTOR_ref::vget(0.35f, -0.25f, 0.9f), VECTOR_ref::vget(0, 0.1f, -0.025f), 15.f);
						return;
					}
					//初段装填
					if (this->gunanime_first->per == 1.f) {
						Func_Set_LEFT_pos_Anim(&this->ani_timeline[6], &this->ani_lefthand[0], VECTOR_ref::vget(0.f, -0.75f, 0.9f), VECTOR_ref::vget(0.05f, -0.035f, -0.055f), -40.f);
						return;
					}
					//グレ投擲
					if (this->throw_grenades) {
						switch (this->throw_gre_ings) {
						case 0:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[12], &this->ani_lefthand[2], VECTOR_ref::vget(0.f, 0.5f, 0.5f), VECTOR_ref::vget(0.f, -0.05f, 0.f), 0.f)) {
								this->throw_gre_ings = 0;
								this->throw_grenades = false;
							}
							return;
						}
						default:
							break;
						}
						return;
					}
					//眺める
					if (this->view_ing) {
						switch (this->view_ings) {
						case 0:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[7], &this->ani_lefthand[2], VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -85.f)) {
								this->view_ings++;
							}
							return;
						}
						case 1:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[8], &this->ani_lefthand[2], VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -95.f)) {
								this->view_ings++;
							}
							return;
						}
						case 2:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[9], &this->ani_lefthand[0], VECTOR_ref::vget(-0.55f, -0.15f, 0.65f), VECTOR_ref::vget(0.05f, 0.1f, -0.2f), 40.f)) {
								this->view_ings++;
							}
							return;
						}
						case 3:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[10], &this->ani_lefthand[0], VECTOR_ref::vget(-0.55f, -0.15f, 0.65f), VECTOR_ref::vget(0.05f, 0.1f, -0.2f), 40.f)) {
								this->view_ings = 0;
								this->view_ing = false;
							}
							return;
						}
						default:
							break;
						}
					}
					//ノーマル
					if (this->HP_parts[2] == 0) {
						Func_Set_LEFT_pos_Anim(&this->ani_timeline[11], &this->ani_lefthand[2], VECTOR_ref::vget(0.f, 0.f, 1.f), VECTOR_ref::vget(0.f, 0.f, 0.f), 0.f);
					}
					else {
						Func_Set_LEFT_pos_Anim(&this->ani_timeline[0], &this->ani_lefthand[0], VECTOR_ref::vget(0.f, 0.f, 1.f), VECTOR_ref::vget(0.f, 0.f, 0.f), 0.f);
					}
					return;
				}
			}
		}
		//アイテムリリース
		void release(MAG_STATUS* mag) noexcept {
			auto& item = MAPPTs->item;
			moves tmp = this->mag_m;
			tmp.vec = (this->base.Get_magf_pos() - this->base.Get_mag2f_pos()).Norm()*1.f / FPS;
			for (auto& i : item) {
				if (i->Set_item_(mag->ptr_mag, tmp, mag->m_cnt)) {
					return;
				}
			}
			item.emplace_back(std::make_shared<Items>(item.size(), mag->ptr_mag, tmp, mag->m_cnt));
		}
		void release(Meds*meddata) noexcept {
			auto& item = MAPPTs->item;
			moves tmp;
			tmp.pos = this->get_maz();
			tmp.mat = this->gun_m.mat;
			tmp.vec = (this->base.Get_objMatrix().zvec()).Norm()*-5.f / FPS;
			for (auto& i : item) {
				if (i->Set_item_(meddata, tmp)) {
					return;
				}
			}
			item.emplace_back(std::make_shared<Items>(item.size(), meddata, tmp));
		}
		void release(Grenades*gredata) noexcept {
			auto& item = MAPPTs->item;
			moves tmp;
			tmp.pos = this->LEFTHAND.move.pos;
			tmp.mat = this->gun_m.mat;
			tmp.vec = (MATRIX_ref::Vtrans(this->HMD.move.mat.zvec(), MATRIX_ref::RotAxis(this->HMD.move.mat.xvec(), deg2rad(15)))).Norm()*-17.5f / FPS;
			for (auto& i : item) {
				if (i->Set_item_(gredata, tmp)) {
					return;
				}
			}
			item.emplace_back(std::make_shared<Items>(item.size(), gredata, tmp));
		}
		//マガジン排出
		void magazine_release(void) noexcept {
			release(&this->gun_stat_now->get_magazine_in().front());//アイテムを落とす
			this->gun_stat_now->get_magazine_in_pop_front();//先頭削除
		}
		void magrelease_t(size_t test_) noexcept {
			if (this->gun_stat_now->hav_mag()) {
				//音
				this->audio.magazine_down.vol(255);
				this->audio.magazine_down.play_3D(this->gun_m.pos, 15.f);
				//弾数
				if (test_ == 0) {//全排出
					while (true) {
						magazine_release();
						if (!this->gun_stat_now->hav_mag()) { break; }
					}
				}
				if (test_ == 1) {//通常排出
					magazine_release();
				}

				if (test_ == 2) {//末尾排出
					release(&this->gun_stat_now->get_magazine_in().back());//アイテムを落とす
					this->gun_stat_now->get_magazine_in_pop_back();//末尾削除
				}
				this->reload_cnt = 0.f;
				return;
			}
		}
		//腕のフレーム操作
		void move_hand(const bool isusewaist, const MATRIX_ref& m_inv, bool ptt, bool LorR) noexcept {
			//左
			if (LorR) {
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((this->LEFTHAND.move.pos - this->BodyFrame(this->frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFThand_f.first) - this->BodyFrame(this->frame_s.LEFTarm2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTarm2_f.first) - this->BodyFrame(this->frame_s.LEFTarm1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTarm1_f.first) - this->LEFTHAND.move.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((ptt && !isusewaist) || !ptt) {
					this->BodyFrameLocalMatrix(this->frame_s.LEFTarm1_f);
				}
				if (ptt && isusewaist) {
					this->BodyFrameLocalMatrix(this->frame_s.LEFTarm1_f,(this->WAIST.move.mat*m_inv.Inverse()).Inverse());
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTarm2_f.first) - this->BodyFrame(this->frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((ptt && !isusewaist) || !ptt) {
					this->BodyFrameLocalMatrix(this->frame_s.LEFTarm1_f, a1_inv);
				}
				if (ptt && isusewaist) {
					this->BodyFrameLocalMatrix(this->frame_s.LEFTarm1_f, a1_inv*(this->WAIST.move.mat*m_inv.Inverse()).Inverse());
				}
				//下腕
				this->BodyFrameLocalMatrix(this->frame_s.LEFTarm2_f);
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFThand_f.first) - this->BodyFrame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->LEFTHAND.move.pos - this->BodyFrame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.LEFTarm2_f, a2_inv);
				//手
				this->BodyFrameLocalMatrix(this->frame_s.LEFThand_f);
				auto ans1_ = this->LEFT_pos_Anim(0);
				auto ans2_ = this->LEFT_pos_Anim(1);
				auto ans3_ = this->LEFT_pos_Anim(2);
				MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFThand2_f.first) - this->BodyFrame(this->frame_s.LEFThand_f.first), m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()),
					MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.LEFThand_f, a3_inv);
				MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrameMatrix(this->frame_s.LEFThand_f.first).zvec()*-1.f, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()),
					MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.LEFThand_f, a3_yvec * a3_inv);
			}
			//右
			else {
				//基準
				this->RIGHTHAND.move.pos = this->RIGHT_pos_gun(0);
				if (!ptt) {
					this->RIGHTHAND.move.mat = this->gun_m.mat;
				}
				auto vec_a1 = MATRIX_ref::Vtrans((this->RIGHTHAND.move.pos - this->BodyFrame(this->frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(-1.f*sin(deg2rad(30)), -1.f*cos(deg2rad(30)), vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHThand_f.first) - this->BodyFrame(this->frame_s.RIGHTarm2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTarm2_f.first) - this->BodyFrame(this->frame_s.RIGHTarm1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTarm1_f.first) - this->RIGHTHAND.move.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((ptt && !isusewaist) || !ptt) {
					this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm1_f);
				}
				if (ptt && isusewaist) {
					this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm1_f, (this->WAIST.move.mat*m_inv.Inverse()).Inverse());
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTarm2_f.first) - this->BodyFrame(this->frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((ptt && !isusewaist) || !ptt) {
					this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm1_f, a1_inv);
				}
				if (ptt && isusewaist) {
					this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm1_f, a1_inv*(this->WAIST.move.mat*m_inv.Inverse()).Inverse());
				}
				//下腕
				this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm2_f);
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHThand_f.first) - this->BodyFrame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->RIGHTHAND.move.pos - this->BodyFrame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.RIGHTarm2_f, a2_inv);
				//手
				this->BodyFrameLocalMatrix(this->frame_s.RIGHThand_f);
				auto ans1_ = this->RIGHT_pos_gun(0);
				auto ans2_ = this->RIGHT_pos_gun(1);
				auto ans3_ = this->RIGHT_pos_gun(2);
				MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHThand2_f.first) - this->BodyFrame(this->frame_s.RIGHThand_f.first), m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()),
					MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.RIGHThand_f, a3_inv);
				MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrameMatrix(this->frame_s.RIGHThand_f.first).zvec()*-1.f, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()),
					MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()));
				this->BodyFrameLocalMatrix(this->frame_s.RIGHThand_f, a3_yvec * a3_inv);
			}
		}
		//手振れ
		const auto get_res_blur(float persent) noexcept {
			if (persent != 1.f) {
				auto v_x = this->blur_vec;
				v_x.y(v_x.y() *persent);
				v_x = v_x.Norm();

				auto v_z = this->recoil_vec;
				v_z.y(v_z.y()*persent);
				v_z = v_z.Norm();

				return MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), v_x)* MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), v_z);
			}
			return MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), this->blur_vec)* MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), this->recoil_vec);
		}
		//頭の揺れ
		const auto Head_bobbing(MV1::ani* ani_ptr) noexcept { return sin(DX_PI_F*2.f*(ani_ptr->time / ani_ptr->alltime)) *ani_ptr->per; }
		//銃器の位置指定(右手が沿うため)
		void move_nomal_gun(const std::shared_ptr<PLAYERclass::Chara>& mine) noexcept {
			{
				{
					//サイト位置決定
					VECTOR_ref sightv = this->base.Get_sight_frame();//サイト
					if (select_sight != SIZE_MAX) {
						sightv = this->sight_[select_sight].Get_sightvec();//サイト
					}
					//指定
					if (this->ads_on()) {
						easing_set(&this->gunpos,
							VECTOR_ref::vget(-0.035f - sightv.x(), -sightv.y() + Head_bobbing(this->anime_walk)*0.001f, -0.15f),
							std::min(0.75f + ((0.9f - 0.75f)*(this->per_all.weight - this->base.thisparts->per.weight) / 3.f), 0.935f));
					}
					else {
						easing_set(&this->gunpos,
							VECTOR_ref::vget(-0.15f - sightv.x(), -0.07f - sightv.y() + Head_bobbing(this->anime_walk)*0.002f, ((this == mine.get()) ? -0.09f : -0.15f)),
							0.75f);
					}
				}
				//視点を一時取得(getpos対処)
				set_HMDpos();
				//通常
				this->gun_m.mat = get_res_blur(1.f)*this->HMD.move.mat;//リコイル
				this->gun_m.pos = this->get_pos() + MATRIX_ref::Vtrans(this->gunpos, this->gun_m.mat);
#ifdef _USE_OPENVR_
				if (DrawPts->use_vr) {
					this->gun_m.pos -= this->HMD.move_start.pos;
				}
#endif // _USE_OPENVR_
				//左右の揺れ
				moves tmp;
				tmp.pos = this->gun_m.pos;
				tmp.mat = MATRIX_ref::RotZ(this->body_ztrun)* MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), VECTOR_ref::vget(this->body_ztrun / 1.5f, this->body_xtrun * 1.5f, 1.f))* this->gun_m.mat;
				set_gun_pos(tmp);
				//リロード
				tmp.pos = this->gun_m.pos + MATRIX_ref::Vtrans(pos_gunani, this->gun_m.mat);
				tmp.mat = MATRIX_ref::RotZ(deg2rad(zrad_gunani))* MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), vec_gunani)* this->gun_m.mat;
				set_gun_pos(tmp);
				//壁沿い
				{
					int radd = 0;
					MATRIX_ref mat_gun_old = this->gun_m.mat;
					while (true) {
						this->Set_gun(true);//一時的
						if (MAPPTs->map_col_line(this->RIGHT_pos_gun(0), this->get_maz()).HitFlag == TRUE) {
							tmp.pos = this->gun_m.pos + mat_gun_old.zvec()*0.006f;
							tmp.mat = MATRIX_ref::RotX(deg2rad(2))* this->gun_m.mat;
							set_gun_pos(tmp);//ひっこめ(仮)
						}
						else {
							break;
						}
						radd += 2;
						if (radd > 60) {
							break;
						}
					}
					easing_set(&this->rad_gun, float(radd), 0.9f);
					tmp.pos = this->gun_m.pos;
					tmp.mat = MATRIX_ref::RotX(deg2rad(this->rad_gun))* mat_gun_old;
					set_gun_pos(tmp);//ひっこめ
				}
				this->Set_gun();//一時的に銃位置更新
			}
		}
		//自分についているパーツがある場合削除
		void Detach_child(GPARTS_COMMON* parent_ptr, size_t type_t, int sight_s = 0) noexcept {
			if (parent_ptr == nullptr) {
				return;
			}
			if (!parent_ptr->IsActive()) {
				return;
			}
			{
				if (this->mazzule.parent == parent_ptr) { this->mazzule.Detach(this->per_all); }
				if (this->grip.parent == parent_ptr) { this->grip.Detach(this->per_all); }
				if (this->uperhandguard.parent == parent_ptr) { this->uperhandguard.Detach(this->per_all); }
				if (this->underhandguard.parent == parent_ptr) { this->underhandguard.Detach(this->per_all); }
				if (this->mount_base.parent == parent_ptr) { this->mount_base.Detach(this->per_all); }
				if (this->dustcover.parent == parent_ptr) { this->dustcover.Detach(this->per_all); }
				if (this->stock.parent == parent_ptr) { this->stock.Detach(this->per_all); }
				//
				if (this->mount_.parent == parent_ptr) { this->mount_.Detach(this->per_all); }
				for (auto& s : this->sight_) { if (s.parent == parent_ptr) { s.Detach(this->per_all); } }
				if (this->foregrip.parent == parent_ptr) { this->foregrip.Detach(this->per_all); }
				if (this->lam.parent == parent_ptr) { this->lam.Detach(this->per_all); }
				//
				switch (type_t) {
				case PARTS_SIGHT:
				{
					if (this->sight_.size() > sight_s) { this->sight_[sight_s].Detach(this->per_all); }
					break;
				}
				default:
				{
					parent_ptr->Detach(this->per_all);
					break;
				}
				}
			}
		}
		//VR操作
		void vr_move(void) {
#ifdef _USE_OPENVR_
			if (DrawPts->get_hand2_num() != -1) {
				auto ptr_ = DrawPts->get_device_hand2();
				if (ptr_->turn && ptr_->now) {
					//移動
					if ((ptr_->on[1] & BUTTON_TOUCHPAD) != 0) {
						this->speed = (this->key_.running ? 8.f : 6.f) / FPS;

						if (DrawPts->tracker_num.size() > 0) {
							auto p = this->BodyFrameMatrix(this->frame_s.bodyb_f.first);
							easing_set(&this->add_vec_buf, (p.zvec()*-ptr_->touch.y() + p.xvec()*-ptr_->touch.x())*this->speed, 0.95f);
						}
						else {
							easing_set(&this->add_vec_buf, (this->HMD.move.mat.zvec()*ptr_->touch.y() + this->HMD.move.mat.xvec()*ptr_->touch.x())*this->speed, 0.95f);
						}
					}
					else {
						easing_set(&this->add_vec_buf, VECTOR_ref::vget(0, 0, 0), 0.95f);
					}
				}
			}
#endif // _USE_OPENVR_
		}
		//Nomal操作
		void key_move(float& x_m, float& y_m, const float fov_per) {
			int x_t, y_t;
			GetMousePoint(&x_t, &y_t);//~0.01
			x_m = float(std::clamp(x_t - deskx / 2, -120, 120))*fov_per;
			y_m = float(std::clamp(y_t - desky / 2, -120, 120))*fov_per;
			SetMousePoint(deskx / 2, desky / 2);//ウィンドウだと0.05～0.10ms掛かる?!
			SetMouseDispFlag(FALSE);
		}
		//AI操作
		void AI_move(float& x_t, float& y_t, const std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara) {
			int32_t x_m, y_m;
			//AI
			int turn = -1;
			bool is_player = false;
			//操作
			{
				this->key_.wkey = false;
				this->key_.skey = false;
				//this->akey = false;
				//this->dkey = false;
				this->key_.shoot = false;
				this->key_.running = false;
				this->key_.aim = false;
				//this->TPS = false;
				this->key_.reload = false;
				this->key_.have_item = false;
				this->key_.sort_magazine = false;
				this->key_.view_gun = false;
				this->key_.drop_ = false;
				this->key_.throw_gre = false;
				this->key_.select = false;
				this->key_.have_mag = true;
				this->key_.jamp = false;
				this->key_.squat = false;
			}
			//
			VECTOR_ref vec_to;
			//
			auto poss = this->get_pos();
			auto vec_mat = this->BodyFrameMatrix(this->frame_s.head_f.first);
			auto vec_x = vec_mat.xvec();
			auto vec_y = vec_mat.yvec();
			auto vec_z = vec_mat.zvec()*-1.f;
			//狙うキャラを探索+AIのフェーズ選択
			{
				bool pp = true;
				{
					auto StartPos = this->get_head_pos();
					VECTOR_ref EndPos;
					for (auto& tgt : chara) {
						if (this == tgt.get()) { continue; }
						const size_t index = &tgt - &chara.front();
						if (tgt->flag_gun) { turn = int(index); }
						EndPos = (this->cpu_do.ai_time_shoot < 0.f) ? tgt->get_head_pos() : tgt->BodyFrame(tgt->frame_s.bodyb_f.first);
						if (vec_to == VECTOR_ref::vget(0.f, 0.f, 0.f)) { vec_to = EndPos - StartPos; }//基準の作成
						if (!tgt->get_alive()) { continue; }
						if (MAPPTs->map_col_line(StartPos, EndPos).HitFlag == TRUE) { continue; }
						EndPos = EndPos - StartPos;
						if (vec_to.size() >= EndPos.size()) {
							vec_to = EndPos;
							is_player = (index == 0);
							pp = false;
						}
					}
				}
				auto ai_phase_old = this->cpu_do.ai_phase;
				if (pp) {
					this->cpu_do.ai_phase = 0;
				}
				else {
					if (vec_z.dot(vec_to.Norm()) >= 0 && vec_to.size() <= 60.f) {
						this->cpu_do.ai_phase = 1;
					}
				}
				if (isReload()) {
					this->cpu_do.ai_phase = 2;
				}
				if ((ai_phase_old == 1 && this->cpu_do.ai_phase != 1) || (this->add_vec_real.size() <= this->move.vec.size()*0.8f)) {
					int now = MAPPTs->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
			}
			//
			if (this->key_.ekey) {
				this->cpu_do.ai_time_e += 1.f / FPS;
				if (this->cpu_do.ai_time_e >= 2) {
					this->key_.ekey = false;
					this->cpu_do.ai_time_e = 0.f;
				}
			}
			for (const auto& w : MAPPTs->get_leanpoint_e()) {
				if ((w - poss).size() <= 0.5f) {
					this->key_.ekey = true;
					this->key_.qkey = false;
					this->cpu_do.ai_time_e = 0.f;
					break;
				}
			}
			if (this->key_.qkey) {
				this->cpu_do.ai_time_q += 1.f / FPS;
				if (this->cpu_do.ai_time_q >= 2) {
					this->key_.qkey = false;
					this->cpu_do.ai_time_q = 0.f;
				}
			}
			for (const auto& w : MAPPTs->get_leanpoint_q()) {
				if ((w - poss).size() <= 0.5f) {
					this->key_.ekey = false;
					this->key_.qkey = true;
					this->cpu_do.ai_time_q = 0.f;
					break;
				}
			}
			//
			switch (this->cpu_do.ai_phase) {
			case 0://通常フェイズ
			{
				this->cpu_do.ai_reload = false;

				this->key_.akey = false;
				this->key_.dkey = false;
				if (this->cpu_do.ai_time_walk < 4.f) {
					this->key_.wkey = true;
				}
				{
					if (this->cpu_do.ai_time_walk >= 6.f) {
						this->cpu_do.ai_time_walk = 0.f;
					}
					this->cpu_do.ai_time_walk += 1.f / FPS;
				}

				this->cpu_do.ai_time_shoot = 0.f;
				//向き指定
				if (turn >= 0) {
					vec_to = chara[turn]->get_pos() - poss;
				}
				else {
					vec_to = MAPPTs->get_waypoint()[this->cpu_do.wayp_pre.front()] - poss;
					vec_to.y(0.f);
				}
				//到達時に判断
				if (vec_to.size() <= 0.5f) {
					int now = MAPPTs->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
				x_m = -int(vec_x.dot(vec_to.Norm()) * 120);
				y_m = -int(vec_y.dot(vec_to.Norm()) * 10);
			}
			break;
			case 1://戦闘フェイズ
			{
				this->cpu_do.ai_reload = false;
				int range = int(1000.f + 1000.f*vec_to.size() / 20.f);
				x_m = -int(vec_x.dot(vec_to) * 25) + int(float(-range + GetRand(range * 2)) / 100.f);
				y_m = -int(vec_y.dot(vec_to) * 25) + int(float(GetRand(range * 2)) / 100.f);

				this->cpu_do.ai_time_shoot += 1.f / FPS;
				if (this->cpu_do.ai_time_shoot < 0.f) {
					this->key_.akey = false;
					this->key_.dkey = false;
					if (this->cpu_do.ai_time_shoot >= -5) {
						if (is_player) {
							this->key_.shoot = GetRand(100) <= 8;
						}
						else {
							this->key_.shoot = GetRand(100) <= 20;
						}
						this->key_.aim = true;
						this->key_.squat = true;
					}
				}
				else {
					if (is_player) {
						this->key_.shoot = GetRand(100) <= 20;
					}
					else {
						this->key_.shoot = GetRand(100) <= 50;
					}

					if (this->cpu_do.ai_time_shoot >= GetRand(20) + 5) {
						this->cpu_do.ai_time_shoot = float(-8);
					}
					if (this->key_.ekey && !this->key_.akey) {
						this->key_.akey = true;
						this->key_.dkey = false;
						this->cpu_do.ai_time_d = 0.f;
						this->cpu_do.ai_time_a = 0.f;
					}
					if (this->key_.qkey && !this->key_.dkey) {
						this->key_.dkey = true;
						this->key_.akey = false;
						this->cpu_do.ai_time_d = 0.f;
						this->cpu_do.ai_time_a = 0.f;
					}

					if (!this->key_.akey) {
						this->cpu_do.ai_time_d += 1.f / FPS;
						if (this->cpu_do.ai_time_d > GetRand(3) + 1) {
							this->key_.akey = true;
							this->cpu_do.ai_time_d = 0.f;
						}
					}
					else {
						this->cpu_do.ai_time_a += 1.f / FPS;
						if (this->cpu_do.ai_time_a > GetRand(3) + 1) {
							this->key_.akey = false;
							this->cpu_do.ai_time_a = 0.f;
						}
					}
					this->key_.dkey = !this->key_.akey;
				}
				if (!this->gun_stat_now->not_chamber_EMPTY()) {
					if (!isReload()) {
						this->key_.reload = true;
						this->cpu_do.ai_reload = true;
					}
				}
			}
			break;
			case 2://リロードフェイズ
			{
				this->key_.akey = false;
				this->key_.dkey = false;

				this->key_.wkey = true;
				this->key_.running = true;

				if (this->cpu_do.ai_reload) {
					this->cpu_do.set_wayp_return();
					this->cpu_do.ai_reload = false;
				}
				//向き指定
				vec_to = MAPPTs->get_waypoint()[this->cpu_do.wayp_pre.front()] - poss;
				vec_to.y(0.f);
				//到達時
				if (vec_to.size() <= 0.5f) {
					int now = MAPPTs->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
				x_m = -int(vec_x.dot(vec_to.Norm()) * 40);
				y_m = -int(vec_y.dot(vec_to.Norm()) * 40);
			}
			break;
			default:
				break;
			}
			//y～z 0.00ms
			x_m = std::clamp(x_m, -40, 40);
			y_m = std::clamp(y_m, -40, 40);
			x_t = float(x_m);
			y_t = float(y_m);
		}
	private:
		//操作
		void operation(const bool cannotmove, const std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara, const float fov_per, uint8_t move_mode) noexcept {
			float x_m = 0.f, y_m = 0.f;
			if (this->get_alive()) {
				if (this->get_alive()) {
					switch (move_mode) {
					case 0://player move
						if (DrawPts->use_vr) {
							vr_move();//player move(VR)
						}
						else {
							key_move(x_m, y_m, fov_per);//player move
						}
						break;
					case 1://AI move
						AI_move(x_m, y_m, chara);
						break;
					default://cannot move
						this->key_.ReSet_();
						break;
					}
				}
			}
			if (cannotmove || !this->get_alive()) {
				this->key_.ReSet_();
				//移動
				easing_set(&this->add_vec_buf, VECTOR_ref::vget(0, 0, 0), 0.95f);
			}
#ifdef _USE_OPENVR_
			if (DrawPts->use_vr) {
				//HMD_mat+視点取得
				this->HMD.move.repos = this->HMD.move.pos;
				this->HMD.UpDate_pos(DrawPts->get_hmd_num(), flag_start_loop);
			}
			else
#endif // _USE_OPENVR_
			{
				this->key_.rule_();
				if (this->get_alive()) {
					easing_set(&this->body_ztrun, -deg2rad(x_m * 25 / 120)*1.f, 0.9f);
					easing_set(&this->body_xtrun, -deg2rad(y_m * 25 / 120)*1.f, 0.9f);
					//z軸回転(リーン)
					if (this->key_.qkey) {
						easing_set(&this->body_zrad, deg2rad(-25), 0.9f);
					}
					else if (this->key_.ekey) {
						easing_set(&this->body_zrad, deg2rad(25), 0.9f);
					}
					else {
						easing_set(&this->body_zrad, this->body_ztrun, 0.8f);
					}
					//y軸回転(旋回)
					this->HMD.mat_notlean = MATRIX_ref::RotX(-this->xrad_p)*this->HMD.mat_notlean;
					this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m)*0.1f, deg2rad(-80), deg2rad(60));
					this->HMD.mat_notlean *= MATRIX_ref::RotY(deg2rad(x_m)*0.1f);
					//x軸回転(仰俯)
					this->HMD.mat_notlean = MATRIX_ref::RotX(this->xrad_p)*this->HMD.mat_notlean;
					//anser
					this->HMD.move.mat = this->HMD.mat_notlean * MATRIX_ref::RotAxis(this->HMD.mat_notlean.zvec(), this->body_zrad);
				}
				this->speed = (this->key_.running ? 6.f : ((this->ads_on() ? 2.f : 4.f)*(this->key_.squat ? 0.7f : 1.f))) / FPS;
				auto zv_t = this->HMD.move.mat.zvec();
				zv_t.y(0.f);
				zv_t = zv_t.Norm();
				auto xv_t = this->HMD.move.mat.xvec();
				xv_t.y(0.f);
				xv_t = xv_t.Norm();
				if (this->key_.running) {
					xv_t = xv_t.Norm()*0.5f;
				}
				easing_set(&this->add_vec_buf,
					VECTOR_ref(VECTOR_ref::vget(0, 0, 0))
					+ (this->key_.wkey ? (zv_t*-this->speed) : VECTOR_ref::vget(0, 0, 0))
					+ (this->key_.skey ? (zv_t*this->speed) : VECTOR_ref::vget(0, 0, 0))
					+ (this->key_.akey ? (xv_t*this->speed) : VECTOR_ref::vget(0, 0, 0))
					+ (this->key_.dkey ? (xv_t*-this->speed) : VECTOR_ref::vget(0, 0, 0))
					, 0.95f);
				this->mat_body = this->HMD.move.mat;
			}
			//ジャンプ
			{
				if (!this->get_jamping()) {
					if (this->key_.jamp && this->get_alive()) {
						this->add_ypos = 0.05f*FRAME_RATE / FPS;
					}
					this->move.vec = this->add_vec_buf;
				}
				else {
					easing_set(&this->move.vec, VECTOR_ref::vget(0, 0, 0), 0.995f);
				}
			}
			//操作
			{
				if (isReload()) {
					this->key_.aim = false;
				}
				//引き金(左クリック)
				easing_set(&this->gunanime_trigger->per, float(this->key_.shoot && !this->key_.running), 0.5f);
				//マグキャッチ(Rキー)
				easing_set(&this->gunanime_magcatch->per, float(this->key_.reload), 0.5f);
			}
			//
		}
		//壁、床判定
		void wall_col() noexcept {
			VECTOR_ref pos_t2, pos_t;
			//VR用
#ifdef _USE_OPENVR_
			if (DrawPts->use_vr) {
				pos_t2 = this->pos_tt + (this->HMD.get_pos_old_noY() - this->HMD.move_start.pos);
				pos_t = this->get_pos(true);
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				this->pos_tt = pos_t - (this->HMD.get_pos_noY() - this->HMD.move_start.pos);
			}
#endif // _USE_OPENVR_
			//共通
			{
				pos_t2 = this->pos_tt;
#ifdef _USE_OPENVR_
				if (DrawPts->use_vr) {
					pos_t2 -= this->HMD.move_start.pos;
				}
#endif // _USE_OPENVR_

				//pos_t = pos_t2 + (this->move.vec / 2);
				pos_t = pos_t2 + this->move.vec;
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				//落下
				{
					auto pp = MAPPTs->map_col_line(pos_t + VECTOR_ref::vget(0, 1.8f, 0), pos_t + VECTOR_ref::vget(0, -0.01f, 0));
					if (this->add_ypos <= 0.f && pp.HitFlag == TRUE) {
						pos_t = pp.HitPosition;
						this->add_ypos = 0.f;
					}
					else {
						pos_t.yadd(this->add_ypos);
						this->add_ypos += M_GR / std::powf(FPS, 2.f);
						//復帰
						if (pos_t.y() <= -5.f) {
							this->HP = 0;
							this->scores.set_death(0);
						}
					}
				}
				//反映
				this->pos_tt = pos_t;
#ifdef _USE_OPENVR_
				if (DrawPts->use_vr) {
					this->pos_tt += this->HMD.move_start.pos;
				}
#endif // _USE_OPENVR_
				this->add_vec_real = pos_t - pos_t2;
			}
		}
	private:
		//体の動作を決定
		void Set_body(const std::shared_ptr<PLAYERclass::Chara>& mine) noexcept {
			bool useVR = DrawPts->use_vr && (this == mine.get());
			//
			if (this->get_alive()) {
#ifdef _USE_OPENVR_
				if (useVR) {
					{
						auto v_ = this->HMD.move.mat.zvec();
						if (DrawPts->tracker_num.size() > 0) {
							v_ = this->WAIST.move.mat.zvec();
						}
						float x_1 = -sinf(this->body_yrad);
						float y_1 = cosf(this->body_yrad);
						float x_2 = v_.x();
						float y_2 = -v_.z();
						this->body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / FPS / 10.f;
					}
				}
				else
#endif // _USE_OPENVR_
				{
					{
						auto v_ = this->mat_body.zvec();
						float x_1 = -sinf(this->body_yrad);
						float y_1 = cosf(this->body_yrad);
						float x_2 = v_.x();
						float y_2 = -v_.z();
						this->body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / FPS / 2.5f;
					}
					{
						auto v_ = this->mat_body.zvec();
						float x_1 = sinf(this->body_xrad);
						float y_1 = -cosf(this->body_xrad);
						float x_2 = -v_.y();
						float y_2 = -std::hypotf(v_.x(), v_.z());
						this->body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
					}
				}
			}
			//
			if (useVR) {
#ifdef _USE_OPENVR_
				//身体,頭部,腰
				MATRIX_ref m_inv = MATRIX_ref::RotY((DrawPts->tracker_num.size() > 0) ? DX_PI_F : 0 + this->body_yrad);
				{
					if (this->get_alive()) {
						this->obj_body.SetMatrix(m_inv);
						if (DrawPts->tracker_num.size() > 0) {
							//腰
							this->BodyFrameLocalMatrix(this->frame_s.bodyb_f, this->WAIST.move.mat*m_inv.Inverse());
							//頭部
							this->BodyFrameLocalMatrix(this->frame_s.head_f, this->HMD.get_mat_XZ() *m_inv.Inverse()*(this->WAIST.move.mat*m_inv.Inverse()).Inverse());
						}
						else {
							//頭部
							this->BodyFrameLocalMatrix(this->frame_s.head_f, this->HMD.get_mat_XZ() *m_inv.Inverse());
						}
						this->obj_body.SetMatrix(m_inv *MATRIX_ref::Mtrans(this->get_pos() - (this->BodyFrame(this->frame_s.RIGHTeye_f.first) + (this->BodyFrame(this->frame_s.LEFTeye_f.first) - this->BodyFrame(this->frame_s.RIGHTeye_f.first))*0.5f)));
					}
					else {
						m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
						this->obj_body.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->get_pos(true)));
					}
				}
				//脚関連
				//手関連
				if (this->get_alive()) {
					//右手
					this->RIGHTHAND.UpDate_none(DrawPts->get_hand1_num());
					this->RIGHTHAND.move.mat = this->RIGHTHAND.move.mat*MATRIX_ref::RotAxis(this->RIGHTHAND.move.mat.xvec(), deg2rad(-60));
					//左手
					this->LEFTHAND.UpDate_none(DrawPts->get_hand2_num());
					this->LEFTHAND.move.mat *= MATRIX_ref::RotAxis(this->LEFTHAND.move.mat.xvec(), deg2rad(-60));

					this->RIGHTHAND.move.mat = get_res_blur(1.f) * this->RIGHTHAND.move.mat;//リコイル
				}
				{}
				{
					//頭部
					//足
					{
						//左
						if (DrawPts->tracker_num.size() > 1) {

							LEFTREG.UpDate_mat(DrawPts->tracker_num[1], flag_start_loop);
							this->LEFTREG.move.mat = MATRIX_ref::RotY(deg2rad(90 + 60 - 10))* this->LEFTREG.move_start.mat.Inverse()*this->LEFTREG.move.mat;
							this->LEFTREG.move.pos = this->LEFTREG.move.pos + this->pos_tt - this->HMD.move_start.pos;
							{
								//基準
								auto tgt_pt = this->LEFTREG.move.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->LEFTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot1_f);
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot1_f, a1_inv);

								//下腕
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot2_f);
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot2_f, a2_inv);
								//手
								this->BodyFrameLocalMatrix(this->frame_s.LEFTreg_f, this->LEFTREG.move.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse());
							}

							{
								/*
								auto pp = MAPPTs->map_col_line(this->BodyFrame(this->frame_s.LEFTreg2_f.first) + VECTOR_ref::vget(0, 1.8f, 0), this->BodyFrame(this->frame_s.LEFTreg2_f.first));
								if (pp.HitFlag == TRUE) {
									this->LEFTREG.pos = VECTOR_ref(pp.HitPosition) - (this->BodyFrame(this->frame_s.LEFTreg2_f.first) - this->BodyFrame(this->frame_s.LEFTreg_f.first));
								}
								//*/
							}

							{
								//基準
								auto tgt_pt = this->LEFTREG.move.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->LEFTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot1_f);
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot1_f, a1_inv);

								//下腕
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot2_f);
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->BodyFrameLocalMatrix(this->frame_s.LEFTfoot2_f, a2_inv);
								//手
								this->BodyFrameLocalMatrix(this->frame_s.LEFTreg_f, this->LEFTREG.move.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse());
							}
						}
						//右
						if (DrawPts->tracker_num.size() > 2) {
							this->RIGHTREG.UpDate_mat(DrawPts->tracker_num[2], flag_start_loop);
							this->RIGHTREG.move.mat = MATRIX_ref::RotY(deg2rad(180 - 22 - 10))* this->RIGHTREG.move_start.mat.Inverse()*this->RIGHTREG.move.mat;
							this->RIGHTREG.move.pos += this->pos_tt - this->HMD.move_start.pos;
							{
								//基準
								auto tgt_pt = this->RIGHTREG.move.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->RIGHTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする


								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot1_f);
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot1_f, a1_inv);
								//下腕
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot2_f);
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot2_f, a2_inv);
								//手
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTreg_f, this->RIGHTREG.move.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse());
							}

							{
								/*
								auto pp = MAPPTs->map_col_line(this->BodyFrame(this->frame_s.RIGHTreg2_f.first) + VECTOR_ref::vget(0, 1.8f, 0), this->BodyFrame(this->frame_s.RIGHTreg2_f.first));
								if (pp.HitFlag == TRUE) {
									this->RIGHTREG.pos = VECTOR_ref(pp.HitPosition) - (this->BodyFrame(this->frame_s.RIGHTreg2_f.first) - this->BodyFrame(this->frame_s.RIGHTreg_f.first));
								}
								*/
							}
							{
								//基準
								auto tgt_pt = this->RIGHTREG.move.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->RIGHTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot1_f);
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot1_f, a1_inv);
								//下腕
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot2_f);
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTfoot2_f, a2_inv);
								//手
								this->BodyFrameLocalMatrix(this->frame_s.RIGHTreg_f, this->RIGHTREG.move.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse());
							}
						}
					}
					//手+銃器
					{}
					{
						//銃器の一時的な位置指定(右手が沿うため)
						if (this->get_alive()) { this->set_gun_pos(this->RIGHTHAND.move); }
						this->Set_gun();
						this->Set_GUN_pos_Anim();
						//右手
						move_hand(DrawPts->tracker_num.size() > 0, m_inv, true, false);
						//左手
						this->LEFT_hand = (this->LEFTHAND.move.pos - this->LEFT_pos_Anim(0)).size() <= 0.1f && (!isReload() || !this->key_.have_mag);
						if (this->LEFT_hand) { this->LEFTHAND.move.pos = this->LEFT_pos_Anim(0); }
						move_hand(DrawPts->tracker_num.size() > 0, m_inv, true, true);
						//指
						this->anime_hand_nomal->per = 1.f;
						this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
						this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					}
				}
#endif // _USE_OPENVR_
			}
			else {
				//身体
				MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
				MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(18))*MATRIX_ref::RotZ(this->body_zrad);
				MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(36))*MATRIX_ref::RotZ(this->body_zrad)*MATRIX_ref::RotX(this->body_xrad)*mg_inv;
				//脚関連
				{
					//足の動き
					{
						auto ratio_t = 0.f;
						if (this->key_.running) {
							if (this->key_.wkey || this->key_.skey || this->key_.akey) {
								ratio_t = 1.f;
							}
						}
						else {
							if (this->key_.wkey || this->key_.skey || this->key_.akey || this->key_.dkey) {
								ratio_t = 1.f;
							}
						}
						easing_set(&this->ratio_move, ratio_t, 0.95f);
					}
					//足サウンド
					if (!this->get_jamping()) {
						if (this->key_.running) {
							if (abs(Head_bobbing(this->anime_run)) >= 0.8f && !this->audio.foot_.check()) {
								this->audio.foot_.play_3D(this->gun_m.pos, 15.f);
								this->audio.foot_.vol(255);
							}
						}
						else {
							if (!this->key_.squat) {
								if (abs(Head_bobbing(this->anime_walk)) >= 0.8f && !this->audio.foot_.check()) {
									this->audio.foot_.play_3D(this->gun_m.pos, 5.f);
									this->audio.foot_.vol(128);
								}
							}
							else {
								if (abs(Head_bobbing(this->anime_swalk)) >= 0.8f && !this->audio.foot_.check()) {
									this->audio.foot_.play_3D(this->gun_m.pos, 1.5f);
									this->audio.foot_.vol(64);
								}
							}
						}
					}
					else {
						//this->audio.foot_.stop();
					}
				}
				//手関連
				{
					this->obj_body.frame_reset(this->frame_s.RIGHTarm1_f.first);
					this->obj_body.frame_reset(this->frame_s.RIGHTarm2_f.first);
					this->obj_body.frame_reset(this->frame_s.RIGHThand_f.first);
					this->obj_body.frame_reset(this->frame_s.LEFTarm1_f.first);
					this->obj_body.frame_reset(this->frame_s.LEFTarm2_f.first);
					this->obj_body.frame_reset(this->frame_s.LEFThand_f.first);
					if (this->sort_ing) {
						if (!CheckSoundMem(this->audio.sort_magazine.get()) && !CheckSoundMem(this->audio.load_.get())) {
							this->sort_ing = false;
						}
					}
				}
				//
				if (this->get_alive()) {
					this->obj_body.SetMatrix(MATRIX_ref::Mtrans(this->pos_tt));
					this->BodyFrameLocalMatrix(this->frame_s.bodyg_f, mg_inv);
					this->BodyFrameLocalMatrix(this->frame_s.bodyb_f, mb_inv);
					this->BodyFrameLocalMatrix(this->frame_s.body_f, m_inv*(mb_inv*mg_inv).Inverse());
					this->flag_calc_lag = true;
					//頭部
					{
						this->BodyFrameLocalMatrix(this->frame_s.head_f, this->HMD.move.mat*m_inv.Inverse());
					}
					//足
					{
						float ani_per = 0.95f;//7

						float ani_walk = 0.f;//1
						float ani_run = 0.f;//2
						float ani_sit = 0.f;//7
						float ani_jump = 0.f;//7
						float ani_wake = 0.f;//11
						float ani_swalk = 0.f;//8
						if (this->key_.running) {
							ani_run = this->ratio_move;
						}
						else {
							if (!this->key_.squat) {
								ani_wake = 1.f - this->ratio_move;
								ani_walk = this->ratio_move;
							}
							else {
								ani_sit = 1.f - this->ratio_move;
								ani_swalk = this->ratio_move;
							}
							if (!this->ads_on()) {
								easing_set(&this->anime_hand_nomal->per, 0.f, 0.95f);
							}
						}
						if (this->get_jamping()) {
							ani_jump = 1.f;
							ani_walk = 0.f;//1
							ani_run = 0.f;//2
							ani_sit = 0.f;//7
							ani_wake = 0.f;//11
							ani_swalk = 0.f;//8
							ani_per = 0.8f;
						}

						//しゃがみ
						easing_set(&this->anime_jump->per, ani_jump, ani_per);
						//しゃがみ
						easing_set(&this->anime_sit->per, ani_sit, ani_per);
						//立ち
						easing_set(&this->anime_wake->per, ani_wake, ani_per);
						//走り
						easing_set(&this->anime_run->per, ani_run, ani_per);
						this->anime_run->update(true, 1.f);
						//歩き
						easing_set(&this->anime_walk->per, ani_walk, ani_per);
						this->anime_walk->update(true, 1.f);
						//しゃがみ歩き
						easing_set(&this->anime_swalk->per, ani_swalk, ani_per);
						this->anime_swalk->update(true, ((this->anime_swalk->alltime / 30.f) / this->base.thisparts->reload_time));
					}
					//手+銃器
					{
						//銃器の一時的な位置指定(右手が沿うため)
						move_nomal_gun(mine);
						this->Set_GUN_pos_Anim();
						//右手
						move_hand(false, m_inv, false, false);
						//左手
						this->LEFTHAND.move.pos = this->LEFT_pos_Anim(0);
						this->LEFTHAND.move.mat = this->gun_m.mat;
						this->LEFT_hand = ((this->LEFTHAND.move.pos - this->LEFT_pos_gun(0)).size() <= 0.1f);
						move_hand(false, m_inv, false, true);
						this->gun_m.vec = this->move.vec;
						//指
						this->anime_hand_nomal->per = 1.f;
						this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
						this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					}
				}
				else {
					//銃器
					{
						this->gun_m.Update_Physics();
						auto pp = MAPPTs->map_col_line(this->gun_m.pos + VECTOR_ref::vget(0, 1.f, 0), this->gun_m.pos - VECTOR_ref::vget(0, 0.05f, 0));
						if (pp.HitFlag == TRUE) {
							this->gun_m.HitGround(pp, 0.05f);
							easing_set(&this->gun_m.vec, VECTOR_ref::vget(0, 0, 0), 0.8f);
						}
						this->Set_gun();
					}
				}
			}
			//アニメ更新
			this->obj_body.work_anime();
			//obj演算
			this->Set_LEFT_pos_Anim();
			//銃位置補正
			if (!useVR) {
				if (this->get_alive()) {
					move_nomal_gun(mine);//nomal限定
				}
			}
		}
	public:
		//銃座標指定
		void set_gun_pos(const moves& move_t) noexcept {
			this->gun_m = move_t;
		}
		//
		const MATRIX_ref Mag_Mat(void) noexcept {
			return this->mag_m.mat* MATRIX_ref::Mtrans(this->mag_m.pos);
		}
		const MATRIX_ref Gun_Mat(void) noexcept {
			return this->gun_m.mat * MATRIX_ref::Mtrans(this->gun_m.pos);
		}
		//刺さっている時のマガジン座標
		void set_mag_pos(void) noexcept {
			this->mag_m.mat = this->gun_m.mat;
			this->mag_m.pos = this->base.Get_mag2f_pos();
		}
		/*銃pos決定*/
		void Set_gun(bool needonly=false) noexcept {
			this->base.Setpos_Nomal(this->gun_m.mat * MATRIX_ref::Mtrans(this->gun_m.pos));
			if (needonly) {
				this->mazzule.Setpos_parts(this->gun_m.mat);
			}
			else {
				this->underhandguard.Setpos_parts(this->gun_m.mat);
				this->uperhandguard.Setpos_parts(this->gun_m.mat);
				this->grip.Setpos_parts(this->gun_m.mat);
				this->mazzule.Setpos_parts(this->gun_m.mat);
				this->dustcover.Setpos_parts(this->gun_m.mat);
				this->stock.Setpos_parts(this->gun_m.mat);
				this->foregrip.Setpos_parts(this->gun_m.mat);
				this->lam.Setpos_parts(this->gun_m.mat);
				this->mount_base.Setpos_parts(this->gun_m.mat);
				this->mount_.Setpos_parts(this->gun_m.mat);
				for (auto&s : this->sight_) { s.Setpos_parts(this->gun_m.mat); }
			}
		}
		//刺さっているマガジンのMV1
		auto* mag_obj(void) {
			return &gun_stat_now->get_magazine_in().front().obj_mag_;
		}
		//マガジンの座標確定
		void Set_Magazine_Mat(void) noexcept {
			if (this->gun_stat_now->hav_mag()) {
				auto mat_t = Mag_Mat();
				this->mag_obj()->SetMatrix(mat_t);
				auto& mag_t = gun_stat_now->get_magazine_in().front();
				mag_t.obj_ammo[0].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->mag_obj()->frame(mag_t.magazine_ammo_f[0].first) - mat_t.pos()));
				mag_t.obj_ammo[1].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->mag_obj()->frame(mag_t.magazine_ammo_f[1].first) - mat_t.pos()));
			}
		}
		//パーツアタッチ
		void Attach_parts(GUNPARTs* partsptr, EnumGunParts type_t, GPARTS_COMMON* parents = nullptr, EnumAttachPoint side = POINTS_NONE, size_t mount_t = 0) noexcept {
			if (partsptr != nullptr) {
				switch (type_t) {
				case PARTS_NONE:
				{
					break;
				}
				case PARTS_BASE:
				case PARTS_MAGAZINE:
				{
					this->get_parts(type_t)->attach_(this->per_all, *partsptr, type_t);
					break;
				}
				case PARTS_SIGHT:
				{
					if (parents != nullptr && mount_t > 0) {
						if (this->sight_.size() < mount_t) {
							this->sight_.resize(mount_t);
						}
						this->sight_[mount_t - 1].attach_(this->per_all, *partsptr, type_t, parents, side);
					}
					break;
				}
				default:
					this->get_parts(type_t)->attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
			}
		}
		//パーツ外し
		void Detach_parts(size_t type_t, int mount_t = 0) noexcept {
			switch (type_t) {
			default:
			{
				this->Detach_child(this->get_parts(type_t), type_t);
				break;
			}
			case PARTS_SIGHT:
			{
				if (this->sight_.size() > mount_t) { this->Detach_child(&this->sight_[mount_t], type_t, mount_t); }
				break;
			}
			case PARTS_NONE:
			{
				break;
			}
			}
		}
		//薬莢の生成
		void create_cart(void) noexcept {
			//弾
			moves tmp;
			tmp.pos = this->get_maz();
			tmp.vec = this->gun_m.mat.zvec()*-1.f;
			this->bullet[this->use_bullet].Put(&this->base.thisparts->ammo[0], tmp);
			//薬莢待機
			this->cart[this->use_bullet].Put_first(this->base.thisparts->ammo[0].get_model(), this->base.Get_cate_pos(), this->gun_m.mat);
			//
			++this->use_bullet %= max_bullet;//次のIDへ
		}
		//薬莢の演算
		void calc_cart(const float cart_rate_t) noexcept {
			//薬莢待機
			//auto zpos_buf = this->base.Get_objFrameLocalMatrix(this->base.Get_slide_frame().first).pos().z();
			for (auto& a : this->cart) {
				//待機中の移動
				a.Put_second(this->base.Get_cate_pos(), this->gun_m.mat);
				//if (((this->zpos_cart - zpos_buf)*1000.f) >= -0.1f) {
					//解放
				a.Put_stay(this->base.Get_cate_vec()*2.5f / FPS, cart_rate_t);
				//}
			}
			//clsDx();
			//printfDx("%0.5f", ((this->zpos_cart - zpos_buf)*1000.f));
			//this->zpos_cart = zpos_buf;
		}
		//銃発砲エフェクトのセット
		void calc_shot_effect(void) noexcept {
			switch (this->mazzule.Get_mazzuletype())
			{
			case 0:
				this->effcs[ef_fire].Set(this->get_maz(), this->gun_m.mat.zvec()*-1.f, 0.0025f / 0.1f);//ノーマル
				break;
			case 1:
				this->effcs[ef_fire2].Set(this->get_maz(), this->gun_m.mat.zvec()*-1.f, 0.0025f / 0.1f);//サプ
				break;
			default:
				this->effcs[ef_fire].Set(this->get_maz(), this->gun_m.mat.zvec()*-1.f, 0.0025f / 0.1f);//何もついてない
				break;
			}
		}
		//爆発エフェクトのセット
		void calc_gre_effect(const VECTOR_ref& pos) noexcept {
			this->effcs[ef_greexp].Set(pos, VGet(0.f, 0.f, 1.f), 0.1f / 0.1f);
		}
		//薬莢の処理
		void update_cart(void) noexcept {
			for (auto& a : this->cart) {
				if (a.Get_Flag()) {
					a.UpDate_pos();
					a.UpDate_fall(MAPPTs->map_col_line(a.Get_pos() + VECTOR_ref::vget(0, 1.f, 0), a.Get_pos() - VECTOR_ref::vget(0, 0.008f, 0)), shared_from_this());
				}
			}
		}
		//UI描画用用意
		void Set_Draw_bullet(void) noexcept {
			for (auto& a : this->bullet) { a.Set_Draw(); }
		}
		//銃セレクターのアニメセット
		void Set_select_anime(void) noexcept {
			this->base.thisparts->Set_gun_select(this->gunanime_sel, this->gun_stat_now->get_selecter());
		}
		//銃発砲アニメのセット
		void Set_shot_anime(const float&rate, const bool isNotLast) noexcept {
			if (isNotLast) {
				this->gunanime_shot->per = 1.f;
				if (this->flag_gun) {
					this->gunanime_shot->update(true, rate*2.f*this->gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
					if (this->gunanime_shot->time == 0.f) {
						this->flag_gun = false;
					}
				}
			}
			else {
				this->gunanime_shot_last->per = 1.f;
				this->gunanime_shot_last->update(false, rate*2.f*this->gunanime_shot_last->alltime / (FRAME_RATE / (600.f / 60.f)));
				if (this->gunanime_shot_last->time == 0.f) {
					this->flag_gun = false;
				}
			}
		}
	public:
		//エフェクトの更新
		void update_effect(std::vector<EffekseerEffectHandle>* effsorce) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != ef_smoke) {
					t.put((*effsorce)[index]);
				}
			}
			for (auto& t : this->effcs_gndhit) { t.put((*effsorce)[ef_gndsmoke]); }
		}
	public:
		using PLAYER_COMMON::PLAYER_COMMON;
		void Set(std::vector<GUNPARTs>& gun_data, size_t itr, MV1& body_, MV1& bodylag_, MV1& col_) {
			//身体
			{
				//身体
				body_.DuplicateonAnime(&this->obj_body, &this->obj_body);
				this->frame_s.get_frame(this->obj_body);
				this->anime_hand_nomal = &this->obj_body.get_anime(0);
				this->anime_hand_trigger = &this->obj_body.get_anime(1);
				this->anime_hand_trigger_pull = &this->obj_body.get_anime(2);
				this->anime_swalk = &this->obj_body.get_anime(3);
				this->anime_walk = &this->obj_body.get_anime(4);
				this->anime_run = &this->obj_body.get_anime(5);
				this->anime_wake = &this->obj_body.get_anime(6);
				this->anime_sit = &this->obj_body.get_anime(7);
				this->anime_jump = &this->obj_body.get_anime(8);
				//ラグドール
				bodylag_.DuplicateonAnime(&this->obj_lag, &this->obj_body);
				this->lagframe_.get_frame(this->obj_lag);
				this->flag_calc_lag = true;
				//身体コリジョン
				col_.DuplicateonAnime(&this->obj_col, &this->obj_body);
				this->colframe_.get_frame(this->obj_col);
				for (int i = 0; i < this->obj_col.mesh_num(); ++i) { this->obj_col.SetupCollInfo(8, 8, 8, -1, i); }
			}
			//変数
			{
				this->flag_gun = false;
				this->reloadf = false;
				this->LEFT_hand = false;
				this->recoil_vec = VECTOR_ref::vget(0, 0, 1.f);
				this->recoil_vec_res = this->recoil_vec;
				this->blur_vec = VECTOR_ref::vget(0, 0, 1.f);
				this->blur_vec_res = this->blur_vec;
				this->HP = this->HP_full;
				this->scores.reset(true);
				this->per_all.recoil = 0.f;
				this->per_all.weight = 0.f;
				//腕アニメ
				this->ani_timeline.resize(13);
				this->ani_lefthand.resize(3);
				this->throw_gre_ings = 0;
				this->view_ings = 0;
				this->reload_ings = 0;
				//
				this->HP_parts.resize(this->obj_col.mesh_num());
				for (auto& h : this->HP_parts) { h = 1; }
			}
			//gun
			{
				//base
				Attach_parts(&gun_data[itr], PARTS_BASE);
				//baseanime
				this->gunanime_first = this->base.Get_Anim(0);
				this->gunanime_shot = this->base.Get_Anim(1);
				this->gunanime_shot_last = this->base.Get_Anim(2);
				this->gunanime_trigger = this->base.Get_Anim(3);
				this->gunanime_magcatch = this->base.Get_Anim(4);
				this->gunanime_sel.clear();
				this->gunanime_sel.resize(this->base.thisparts->select.size());
				this->gunanime_sel[0] = this->base.Get_Anim(5);
				this->gunanime_sel[1] = this->base.Get_Anim(6);
				//gunstat
				for (size_t i = 0; i < gun_data.size(); ++i) {
					this->gun_stat.emplace_back(GUN_STATUS());
				}
				this->gun_stat_now = &gun_stat[this->base.thisparts->id_t];
			}
			for (auto& a : this->cart) { a.Set(); }				//薬莢
			for (auto& a : this->bullet) { a.Set(); }			//弾
			this->audio.Duplicate(this->base.thisparts->audio);	//audio
		}
		/*キャラスポーン*/
		void Spawn(const moves& move_) noexcept {
			//
			{
				int sel_s = -1;
				for (auto&s : this->sight_) {
					s.set_sight_vec();
					if (s.IsActive()) {
						sel_s = int(&s - &this->sight_.front());
					}
				}
				select_sight = (sel_s != -1) ? size_t(sel_s) : SIZE_MAX;
			}
			//
			this->xrad_p = 0;
			this->spawn = move_;

			this->pos_tt = this->spawn.pos;
			this->HMD.move.mat = this->spawn.mat;
			this->HMD.mat_notlean = this->HMD.move.mat;

			this->key_.ReSet_();

			this->add_ypos = 0.f;
			this->add_vec_buf.clear();
			this->distance_to_cam = -1.f;
			this->flag_calc_body = true;
			this->flag_calc_lag = true;

			this->HP = this->HP_full;
			this->HP_parts.resize(this->obj_col.mesh_num());
			for (auto& h : this->HP_parts) { h = 1; }

			this->scores.reset(false);

			this->gun_stat_now->Reset();
			for (int i = 0; i < 6; ++i) { this->gun_stat_now->magazine_pushback(this->magazine.thisparts); }

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度

			this->flag_gun = false;
			this->gunanime_shot->reset();
			this->gunanime_shot_last->reset();
			this->gunanime_trigger->reset();
			this->gunanime_magcatch->reset();
			this->gunanime_first->reset();
			this->gunanime_sel[0]->reset();
			this->gunanime_sel[1]->reset();

			this->prev_ani_lefthand = &this->ani_lefthand[0];
			this->next_ani_lefthand = &this->ani_lefthand[0];
			this->nearhit = false;
			//AIの選択をリセット
			{
				int now = MAPPTs->get_next_waypoint(this->cpu_do.wayp_pre, this->get_pos());
				this->cpu_do.Spawn((now != -1) ? now : 0);
			}
		}
		/*最初*/
		void Start(void) noexcept {
#ifdef _USE_OPENVR_
			this->HMD.Set(DrawPts);
			this->WAIST.Set(DrawPts);
			this->LEFTREG.Set(DrawPts);
			this->RIGHTREG.Set(DrawPts);
#endif // _USE_OPENVR_
			this->distance_to_cam = -1.f;
			this->flag_calc_body = false;
			this->flag_start_loop = true;
			this->flag_calc_lag = true;
			this->ratio_move = 0.f;
			this->gunanime_first->per = 1.f;
		}
		//判定起動
		const bool set_ref_col(VECTOR_ref&StartPos, VECTOR_ref& EndPos) {
			//すでに起動しているなら無視
			if (this->nearhit) {
				return true;
			}
			auto aa = this->get_pos();
			if (Segment_Point_MinLen(StartPos, EndPos, aa) <= 2.0f) {
				//判定起動
				this->nearhit = true;
				this->frame_s.copy_frame(this->obj_body, this->colframe_, &this->obj_col);
				for (int i = 0; i < this->obj_col.mesh_num(); ++i) { this->obj_col.RefreshCollInfo(-1, i); }
				return true;
			}
			return false;
		}
		/*更新*/
		void UpDate_(
			HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p,
			const bool playing, const float fov_per,
			std::vector <Meds>& meds_data,
			std::vector<Grenades>& gres_data,
			const std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara, const std::shared_ptr<PLAYERclass::Chara>&mine,
			const std::shared_ptr<PLAYERclass::vehicles>& mine_v, const std::vector<std::shared_ptr<PLAYERclass::vehicles>>& vehicle,
			std::vector<EffekseerEffectHandle>* effsorce
		) noexcept {
			/*
			全体　0.4ms～1.4ms
			*/
			this->nearhit = false;
			{
				easing_set(&this->HP_r, float(this->HP), 0.95f);
				easing_set(&this->got_damage_f, 0.f, 0.95f);
				//
				{
					for (auto& d : this->got_damage_) { easing_set(&d.alpfa, 0.f, 0.975f); }
					while (true) {
						bool none = true;
						for (auto& d : this->got_damage_) {
							if (std::clamp(int(255.f*(1.f - powf(1.f - d.alpfa, 5.f))), 0, 255) < 5) {
								auto i = (&d - &this->got_damage_.front());
								std::iter_swap(this->got_damage_.begin() + i, this->got_damage_.end() - 1);
								this->got_damage_.pop_back();
								none = false;
								break;
							}
						}
						if (none) {
							break;
						}
					}
				}
				if (!this->get_alive()) {
					this->HMD.move.pos.clear();
					easing_set(&this->anime_swalk->per, 0.f, 0.9f);
					easing_set(&this->anime_walk->per, 0.f, 0.9f);
					easing_set(&this->anime_run->per, 0.f, 0.9f);

					easing_set(&this->anime_jump->per, 0.f, 0.9f);
					easing_set(&this->anime_sit->per, 0.f, 0.9f);
					easing_set(&this->anime_wake->per, 0.f, 0.9f);

					easing_set(&this->anime_hand_nomal->per, 0.f, 0.9f);
					easing_set(&this->anime_hand_trigger->per, 0.f, 0.9f);
					easing_set(&this->anime_hand_trigger_pull->per, 0.f, 0.9f);
					//
					if (this->scores.start_death()) {
						this->obj_body.frame_reset(this->frame_s.bodyg_f.first);
						this->obj_body.frame_reset(this->frame_s.bodyb_f.first);
						this->obj_body.frame_reset(this->frame_s.body_f.first);
						this->obj_body.frame_reset(this->frame_s.head_f.first);
						this->obj_body.frame_reset(this->frame_s.RIGHTarm1_f.first);
						this->obj_body.frame_reset(this->frame_s.RIGHTarm1_f.first);
						this->obj_body.frame_reset(this->frame_s.RIGHTarm2_f.first);
						this->obj_body.frame_reset(this->frame_s.RIGHTarm2_f.first);
						this->obj_body.frame_reset(this->frame_s.RIGHThand_f.first);
						this->obj_body.frame_reset(this->frame_s.LEFTarm1_f.first);
						this->obj_body.frame_reset(this->frame_s.LEFTarm1_f.first);
						this->obj_body.frame_reset(this->frame_s.LEFTarm2_f.first);
						this->obj_body.frame_reset(this->frame_s.LEFTarm2_f.first);
						this->obj_body.frame_reset(this->frame_s.LEFThand_f.first);
						magrelease_t(0);												//マガジン全排出
					}
					if (!this->scores.death_f) {
						this->Spawn(this->spawn);
						this->gunanime_shot->per = 1.f;
					}
				}
				this->scores.UpDate();
			}
			//座標取得
#ifdef _USE_OPENVR_
			if (this == mine.get()) {
				if (DrawPts->tracker_num.size() > 0) {
					this->WAIST.UpDate_pos(DrawPts->tracker_num[0], flag_start_loop);
					this->WAIST.move.pos = this->WAIST.move.pos - this->WAIST.move_start.pos;
				}
			}
#endif // _USE_OPENVR_
			//p1～p2 0.00ms
			//プレイヤー操作
			this->operation(!playing, chara, fov_per, (this == mine.get()) ? 0 : 1);
			//p2～p2-1 ～0.11ms
			//壁その他の判定
			this->wall_col();
			//p2-1～p3 ～0.04ms
			//obj演算
			this->Set_body(mine);
//p3～p4 0.15～0.25ms
			//lag演算
			//if (!this->get_alive()) {
				this->frame_s.copy_frame(this->obj_body, this->lagframe_, &this->obj_lag);
				this->obj_lag.work_anime();
			//}
//p4～p4-2 0.03ms
			//obj_col演算
			if (this->get_alive()) {
				this->obj_col.work_anime();
			}
			//p4-2～p4-3 0.01ms
						//
			if (!(DrawPts->use_vr && (this == mine.get()))) {
				//視点取得
				set_HMDpos();
				//銃器
				if (this->get_alive()) {
					Set_gun();
					if (isReload()) {
						auto ans1_ = this->LEFT_pos_Anim(0);
						auto ans2_ = this->LEFT_pos_Anim(1);
						auto ans3_ = this->LEFT_pos_Anim(2);
						this->mag_m.mat = this->gun_m.mat;
						this->mag_m.pos = ans1_;
						Set_Magazine_Mat();
						this->mag_m.mat = MATRIX_ref::Axis1_YZ((ans3_ - ans1_).Norm(), (ans2_ - ans1_).Norm()*-1.f);
						if (this->gun_stat_now->hav_mag()) {
							this->mag_m.pos += (this->mag_m.pos - this->gun_stat_now->get_magazine_in().front().Get_L_mag_pos());
						}
					}
				}
			}
			//p4-3～p5 0.01ms
						//射撃関連
			{
				if (this->gunanime_first->per == 1.f) {
					if ((this->gunanime_first->time > this->gunanime_first->alltime / 3.f) && (!this->audio.slide.check())) {
						this->audio.slide.play_3D(this->gun_m.pos, 15.f);
					}
					this->gunanime_first->update(false, 0.35f);
					if (this->gunanime_first->time >= this->gunanime_first->alltime) {
						this->gunanime_first->reset();
						this->gunanime_shot->per = 1.f;
					}
				}
				//手振れ
				easing_set(&this->blur_vec, this->blur_vec_res, 0.975f);
				//this->blur_vec = VECTOR_ref::vget(0, 0, 1.f);
				easing_set(&this->blur_vec_res, VECTOR_ref::vget(0, 0, 1.f), 0.95f);
				//複座
				easing_set(&this->recoil_vec, this->recoil_vec_res, 0.6f);
				easing_set(&this->recoil_vec_res, VECTOR_ref::vget(0, 0, 1.f), 0.95f);
				//リコイルアニメーション
				if (this->flag_gun) {
					Set_shot_anime(1.f, this->gun_stat_now->not_chamber_EMPTY());
					if (this->gun_stat_now->not_chamber_EMPTY()) {
						this->gunanime_shot_last->reset();
					}
					else {
						this->gunanime_shot->reset();
					}
				}
				//マガジン排出(通常)
				if (this->key_.reload && (!this->reloadf && this->gun_stat_now->hav_mag())) {
					this->reloadf = true;
					magrelease_t(1);
				}
				//medkit生成
				if (this->key_.drop_) {
					release(&meds_data[0]);
				}
				//gre
				if (this->key_.throw_gre) {
					this->throw_grenades = true;
				}

				if ((this->throw_grenade_old != this->throw_grenades) && !this->throw_grenades) {
					release(&gres_data[0]);
				}
				this->throw_grenade_old = this->throw_grenades;
				//眺める
				if (!this->view_ing && this->key_.view_gun) {
					this->view_ing = true;
				}
				//マガジン整頓
				if (!this->sort_ing && this->key_.sort_magazine && this->gun_stat_now->hav_mags()) {
					this->sort_ing = true;
					if (!this->sort_f) {
						this->sort_f = true;
						this->audio.sort_magazine.play_3D(this->gun_m.pos, 15.f);
						this->gun_stat_now->sort_magazine();
					}
					else {
						this->audio.load_.play_3D(this->gun_m.pos, 15.f);
						this->gun_stat_now->load_magazine();
						//マガジン輩出(空)
						if (this->gun_stat_now->get_magazine_in().back().m_cnt == 0) {
							magrelease_t(2);
						}
					}
				}
				//マガジン挿入
				if (isReload()) {
					if (DrawPts->use_vr && this->reload_cnt < this->base.thisparts->reload_time) {
						this->key_.have_mag = false;
					}
					if (this->key_.have_mag) {
						if (this->gun_stat_now->hav_mag()) {
							auto mag0 = this->gun_stat_now->get_magazine_in().front().getmagazine_f(0).first;
							auto mag1 = this->gun_stat_now->get_magazine_in().front().getmagazine_f(1).first;
							if (
								(DrawPts->use_vr) ?
								((this->mag_obj()->frame(mag1) - this->base.Get_magf_pos()).size() <= 0.1f) :
								(this->reload_cnt > this->base.thisparts->reload_time)
								) {
								this->gunanime_shot->per = 1.f;
								this->gunanime_shot_last->reset();
								if (!this->gun_stat_now->not_chamber_EMPTY()) {
									this->gunanime_first->reset();
									this->gunanime_first->per = 1.f;
								}
								this->gun_stat_now->chamber_in();//チャンバーに装填

								this->audio.magazine_Set.play_3D(this->gun_m.pos, 15.f);
								this->reloadf = false;
							}
							if (DrawPts->use_vr) {
								this->mag_m.pos = this->LEFTHAND.move.pos;
								this->mag_m.mat = this->mag_obj()->GetFrameLocalMatrix(mag1) * this->mag_obj()->GetFrameLocalMatrix(mag0) *
									(this->LEFTHAND.move.mat* MATRIX_ref::RotVec2(this->LEFTHAND.move.mat.yvec(), this->base.Get_mag2f_pos() - this->LEFTHAND.move.pos));
							}
						}
						this->reload_cnt += 1.f / FPS;//挿入までのカウント
					}
				}
				else {
					this->key_.have_mag = false;
					set_mag_pos();
				}
				//セレクター
				Set_select_anime();
				if (this->key_.select) {
					size_t tmp = this->gun_stat_now->get_selecter();
					++tmp %= this->base.thisparts->select.size();
					this->gun_stat_now->selector_set(tmp);
				}
				//射撃
				if (!this->flag_gun && this->gun_stat_now->not_chamber_EMPTY()) {
					if (this->base.thisparts->Select_Chose(EnumSELECTER::SELECT_FULL) == int(this->gun_stat_now->get_selecter())) {
						this->trigger.second = 0;
					}
				}
				{
					//
					float xup = 6.f;
					float xdn = -6.f;
					float yup = 6.f;
					float ydn = -6.f;

					switch (this->stock.Get_stocktype()) {
					case 1://チクパなしストック
						xup = 4.f;
						xdn = -4.f;
						yup = 4.f;
						ydn = -4.f;
						break;
					case 2://チクパありストック
						xup = 4.f;
						xdn = -4.f;
						yup = 4.f;
						ydn = -4.f;
						break;
					default://ストック無し
						xup = 6.f;
						xdn = -6.f;
						yup = 6.f;
						ydn = -6.f;
						break;
					}
					auto ppud = (this->LEFT_hand ? 3.f : 1.f) * (this->key_.squat ? 1.75f : 1.0f);							//持ち手を持つとココが相殺される

					easing_set(&this->blur_vec_res,
						MATRIX_ref::Vtrans(this->blur_vec_res,
							MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn*100.f) + GetRand((int32_t)((xup - xdn)*100.f))) / (100.f*ppud)))*
							MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn*100.f) + GetRand((int32_t)((yup - ydn)*100.f))) / (100.f*ppud))))
						, 0.8f);
				}
				this->trigger.GetInput(this->gunanime_trigger->per >= 0.5f);
				if (this->trigger.trigger()) {
					if (!this->flag_gun && !this->audio.trigger.check()) {
						if (this->mazzule.Get_mazzuletype() == 1) {
							this->audio.trigger.vol(164);
						}
						else {
							this->audio.trigger.vol(255);
						}
						this->audio.trigger.play_3D(this->gun_m.pos, 5.f);
					}
					if (!this->flag_gun && this->gun_stat_now->not_chamber_EMPTY() && this->gunanime_first->per == 0.f && !this->view_ing) {
						this->flag_gun = true;
						//弾数管理
						this->gun_stat_now->magazine_shot(isReload());
						{
							float xup = this->base.thisparts->recoil_xup*this->per_all.recoil / 100.f;
							float xdn = this->base.thisparts->recoil_xdn*this->per_all.recoil / 100.f;
							float yup = this->base.thisparts->recoil_yup*this->per_all.recoil / 100.f;
							float ydn = this->base.thisparts->recoil_ydn*this->per_all.recoil / 100.f;

							auto ppud = (this->LEFT_hand ? 3.f : 1.f);// *(this->key_.squat ? 1.75f : 1.0f);							//持ち手を持つとココが相殺される

							this->recoil_vec_res = MATRIX_ref::Vtrans(this->recoil_vec_res,
								MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn*100.f) + GetRand((int32_t)((xup - xdn)*100.f))) / (100.f*ppud)))*
								MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn*100.f) + GetRand((int32_t)((yup - ydn)*100.f))) / (100.f*ppud))));
						}
						//排莢、弾
						create_cart();
						//エフェクト
						calc_shot_effect();
						//サウンド
						{
							if (this->mazzule.Get_mazzuletype() == 1) {
								this->audio.shot.vol(192);
							}
							else {
								this->audio.shot.vol(255);
							}
							this->audio.shot.play_3D(this->gun_m.pos, 100.f);
						}
					}
				}
				//薬莢
				this->calc_cart(1.f);
				//
				this->Set_Magazine_Mat();
				//弾の処理
				for (auto& a : this->bullet) { a.UpDate(shared_from_this(), chara, mine_v, vehicle, hit_obj_p, hit_b_obj_p, MAPPTs); }
				//薬莢の処理
				update_cart();
				//エフェクトの処理
				update_effect(effsorce);
				//銃アニメ更新
				this->base.UpDate_Anim();
			}
			//p5～p5-2 0.08ms
						//アイテム拾う
			MAPPTs->Get_item(this->get_pos_LEFTHAND(), this->get_pos_LEFTHAND() - this->get_mat_LEFTHAND().zvec()*2.6f,shared_from_this(), MAPPTs);
			//p5-2～p5-3 0.01ms
						//物理演算、アニメーション
			{
				if (this->flag_start_loop) {
					this->obj_body.PhysicsResetState();
					this->flag_start_loop = false;
				}
				else {
					if (this->flag_calc_body) {
						this->obj_body.PhysicsResetState();
						this->flag_calc_body = false;
					}
					else {
						this->obj_body.PhysicsCalculation(1000.f / FPS);
					}
				}

				if (this->flag_calc_lag) {
					this->obj_lag.PhysicsResetState();
					this->flag_calc_lag = false;
				}
				else {
					this->obj_lag.PhysicsCalculation(1000.f / FPS);
				}
			}
			//p5-3～p6 0.04ms
						//
			if (this == mine.get()) {
				//ライト
				this->lam.Set_LightHandle(this->base.Get_objMatrix().zvec()*-1.f);
				//息
				if (this->get_alive()) {
					if (this->key_.running) {
						this->audio.voice_breath_run.vol(163);
						if (!this->audio.voice_breath_run.check()) {
							this->audio.voice_breath_run.play_3D(this->get_pos(), 15.f);
						}
					}
					else {
						this->audio.voice_breath.vol(128);
						if (!this->audio.voice_breath.check()) {
							this->audio.voice_breath.play_3D(this->get_pos(), 10.f);
						}
					}

				}
			}
			//p6～p7 0.01ms
		}
		/*視界外か否かを判断*/
		void Check_CameraViewClip(bool use_occlusion) noexcept {
			this->flag_canlook_player = true;
			auto ttt = this->get_pos();

			this->distance_to_cam = (ttt - GetCameraPosition()).size();
			if (CheckCameraViewClip_Box((ttt + VECTOR_ref::vget(-0.6f, 0, -0.6f)).get(), (ttt + VECTOR_ref::vget(0.6f, 1.8f, 0.6f)).get())) {
				this->flag_canlook_player = false;
				this->flag_calc_body = true;
				this->distance_to_cam = -1.f;
				return;
			}
			if (use_occlusion) {
				if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 1.8f, 0)).HitFlag == TRUE &&
					MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.f, 0)).HitFlag == TRUE) {
					this->flag_canlook_player = false;
					this->flag_calc_body = true;
					this->distance_to_cam = -1.f;
					return;
				}
			}
		}
		/*カメラ指定*/
		void Set_cam(cam_info& camera_main, std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara, const float fov_) noexcept {
			if (this->get_alive()) {
				auto mat_T = get_res_blur(0.7f) * this->HMD.move.mat;//リコイル

				auto ppsh = MATRIX_ref::Vtrans(VECTOR_ref::vget(-0.035f, 0, 0), mat_T);
				if (DrawPts->use_vr) {
					mat_T = this->HMD.move.mat;
					ppsh.clear();
				}
				camera_main.set_cam_pos(this->get_pos() + ppsh, this->get_pos() + ppsh + mat_T.zvec()*(DrawPts->use_vr ? 1.f : -1.f), mat_T.yvec());
				camera_main.near_ = 0.1f;
				if (this->ads_on()) {
					easing_set(&camera_main.fov, deg2rad(25), std::min(0.8f + ((0.9f - 0.8f)*(this->per_all.weight - this->base.thisparts->per.weight) / 3.f), 0.925f));
				}
				else {
					easing_set(&camera_main.fov, fov_, 0.9f);
				}
			}
			else {
				//デスカメラ
				if (this->scores.death_id < chara.size()) {
					easing_set(&camera_main.camvec, chara[this->scores.death_id]->get_pos(), 0.9f);
				}
				auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
				easing_set(&camera_main.campos, this->get_pos() + VECTOR_ref::vget(-5.f*sin(rad), 2.f, -5.f*cos(rad)), 0.9f);
				camera_main.camup = VECTOR_ref::vget(0, 1.f, 0);
				MAPPTs->map_col_nearest(camera_main.camvec, &camera_main.campos);
				easing_set(&camera_main.fov, fov_, 0.9f);
			}
		}
		/*キャラ+銃描画*/
		void Draw_chara(int drawlevel = 1) noexcept {
			if (this->flag_canlook_player) {
				switch (drawlevel) {
				case 0://最低レベル
					if (this->get_alive()) {
						this->obj_body.DrawModel();
					}
					else {
						this->obj_lag.DrawModel();
					}
				case 1://低レベル
					if (this->get_alive()) {
						this->obj_body.DrawModel();
					}
					else {
						this->obj_lag.DrawModel();
					}
					//Draw_gun();//drawcall:157
					Draw_gun(0);//drawcall:131
				default://デフォ
					if (this->get_alive()) {
						this->obj_body.DrawModel();
						//this->obj_col.DrawModel();
					}
					else {
						this->obj_lag.DrawModel();
					}
					Draw_gun();
					break;
				}
			}
		}
		//銃描画
		void Draw_gun(int drawlevel = 1) noexcept {
			this->base.Draw();
			if ((!isReload() || this->have_magazine) && this->gun_stat_now->hav_mag()) {
				this->gun_stat_now->get_magazine_in().front().Draw();
			}
			//
			if (drawlevel != 0) {
				if (this->distance_to_cam <= 5.f) {
					for (auto& a : this->cart) { a.Draw(); }
				}
				if (this->distance_to_cam <= 10.f) {
					this->mazzule.Draw();
					this->grip.Draw();
					this->uperhandguard.Draw();
					this->underhandguard.Draw();
					this->mount_base.Draw();
					this->mount_.Draw();
					this->dustcover.Draw();
					this->stock.Draw();
					this->foregrip.Draw();
					this->lam.Draw();
					for (auto&s : sight_) { s.Draw(); }
				}
			}
			//
		}
		/*弾道描画*/
		void Draw_ammo(void) noexcept {
			for (auto& a : this->bullet) { a.Draw(); }
		}
		/*レーザー、ライト描画*/
		void Draw_LAM_Effect(std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara, GraphHandle&light_pic) noexcept {
			switch (this->lam.Get_lamtype(0)) {
			case SELECTLAM_LASER:
			{
				auto StartPos = this->lam.Get_source_pos();
				auto EndPos = StartPos - this->base.Get_objMatrix().zvec()*100.f;
				MAPPTs->map_col_nearest(StartPos, &EndPos);
				for (auto& tgt : chara) {
					if (tgt.get() == this || !tgt->get_alive()) {
						continue;
					}
					if (tgt->set_ref_col(StartPos, EndPos)) {
						for (int i = 0; i < this->obj_col.mesh_num(); ++i) {
							auto q = tgt->obj_col.CollCheck_Line(StartPos, EndPos, -1, i);
							if (q.HitFlag == TRUE) {
								EndPos = q.HitPosition;
							}
						}
					}
				}
				SetUseLighting(FALSE);
				SetFogEnable(FALSE);
				{
					DXDraw::Capsule3D(StartPos, EndPos, 0.001f, GetColor(255, 0, 0), GetColor(255, 255, 255));
					DXDraw::Sphere3D(EndPos, std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.02f), GetColor(255, 0, 0), GetColor(255, 255, 255));
				}
				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
				break;
			}
			case SELECTLAM_LIGHT:
			{
				DrawBillboard3D(this->lam.Get_source_pos().get(), 0.5f, 0.5f, 0.1f, 0.f, light_pic.get(), TRUE);
				break;
			}
			default:
				break;
			}
		}
		/*UI描画*/
		/*HPバーを表示する場所*/
		const auto Set_HP_UI(void) noexcept {
			if ((int(this->HP_r) - this->HP) >= 2) {
				auto head = this->get_head_pos();
				return VECTOR_ref(ConvWorldPosToScreenPos((head + VECTOR_ref::vget(0, 0.3f + 2.7f*std::max((head - GetCameraPosition()).size(), 1.f) / 100.f, 0)).get()));
			}
			else {
				return VECTOR_ref::vget(0.f, 0.f, -1.f);
			}
		}
		/* UI向けにヒット部分を表示*/
		void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
			for (auto& a : this->bullet) {
				if (a.hit_alpha >= 10.f / 255.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f*a.hit_alpha));
					hit_Graph.DrawRotaGraph(a.hit_window_x, a.hit_window_y, a.hit_alpha*0.5f, 0.f, true);//(ismine ? 1.f : 0.5f)
				}
			}
		}
		/*おわり*/
		void Dispose(void) noexcept {
			//パーツリセット
			Detach_parts(PARTS_BASE);
			Detach_parts(PARTS_MAGAZINE);
			Detach_parts(PARTS_DUSTCOVER);
			Detach_parts(PARTS_MAZZULE);
			Detach_parts(PARTS_GRIP);
			Detach_parts(PARTS_STOCK);
			Detach_parts(PARTS_MOUNT_BASE);
			Detach_parts(PARTS_MOUNT);

			Detach_parts(PARTS_UPER_HGUARD);
			Detach_parts(PARTS_UNDER_HGUARD);
			Detach_parts(PARTS_LAM);
			Detach_parts(PARTS_FOREGRIP);

			Detach_parts(PARTS_SIGHT);
			Detach_parts(PARTS_SIGHT);
			Detach_parts(PARTS_SIGHT);
			//
			this->obj_body.Dispose();
			this->obj_col.Dispose();
			//
			this->audio.Dispose();
			for (auto& a : this->cart) { a.Dispose(); }
			for (auto& t : this->effcs) { t.handle.Dispose(); }
			for (auto& t : this->effcs_gndhit) { t.handle.Dispose(); }
			this->gun_stat.clear();
		}
	};

	class vehicles : public std::enable_shared_from_this<vehicles>, public PLAYER_COMMON {
	private:
		//typedef std::pair<size_t, float> pair_hit;
		class pair_hit {
		public:
			size_t first;
			float second;

			//*
			void a() {

			}

		};

		class foot_frame {
		public:
			frames frame;
			float will_y = 0.f;
		};
		class cat_frame {
		public:
			frames frame;
			float will_y = 0.f;
			MV1_COLL_RESULT_POLY colres;
			EffectS gndsmkeffcs;
			float gndsmksize = 1.f;

			cat_frame() {
				frame.first = -1;
				will_y = 0.f;
				gndsmksize = 1.f;
			}
			cat_frame(const cat_frame& tgt) {
				frame = tgt.frame;
				will_y = 0.f;
				gndsmksize = 1.f;
			}
			void operator=(const cat_frame& tgt) {
				frame = tgt.frame;
			}
		};
		//
		class Guns {							/**/
			size_t use_bullet{ 0 };							/*確保する弾*/
			std::array<BULLETS, max_bullet> bullet;			/*確保する弾*/

			float loadcnt{ 0 };					/*装てんカウンター*/
			float fired{ 0.f };					/*駐退数*/
			int16_t rounds{ 0 };				/*弾数*/
			gun_frame gun_info;					/**/
			std::vector<Mainclass::Ammos> Spec;	/**/
		public:
			float xrad = 0.f, yrad = 0.f;

			const float& Getfired() { return fired; }
			const gun_frame& Getgun_info() { return gun_info; }
			const float& Getcaliber(size_t id) { return Spec[id].get_caliber(); }

			void Clear() {
				this->fired = 0.f;
				this->loadcnt = 0.f;
				this->rounds = 0;
				this->use_bullet = 0;
				this->Spec.clear();
			}
			void Set(const gun_frame& resorce) {
				this->gun_info = resorce;
				this->rounds = this->gun_info.get_rounds();
				//使用砲弾
				this->Spec = this->gun_info.get_Spec();
				for (auto& a : this->bullet) { a.Set(); }			//弾
			}
			void SetGunRad(const float view_xrad, const float view_yrad, float limit) {
				this->yrad += std::clamp(view_yrad, -limit, limit);
				//this->yrad = std::clamp(this->yrad + std::clamp(view_yrad, -limit, limit),deg2rad(-30.0)+yrad,deg2rad(30.0)+yrad);//射界制限
				this->xrad = std::clamp(this->xrad + std::clamp(view_xrad, -limit, limit), deg2rad(-10), deg2rad(20));
			}
			template<class Chara, class Map, class vehicles>
			void UpDate(bool key, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t,
				const std::shared_ptr<Chara>& mine, const std::vector<std::shared_ptr<Chara>>& chara,
				const std::shared_ptr<vehicles>& mine_v, const std::vector<std::shared_ptr<vehicles>>& vehicle,
				HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p, std::shared_ptr<Map>& MAPPTs) {
				if (key && this->loadcnt == 0 && this->rounds > 0) {
					auto& u = this->bullet[this->use_bullet];
					++this->use_bullet %= this->bullet.size();
					//ココだけ変化
					moves tmp;
					tmp.pos = pos_t;
					tmp.vec = vec_t.Norm();

					u.Put(&this->Spec[0], tmp);
					this->loadcnt = this->Getgun_info().get_load_time();
					this->rounds = std::max<uint16_t>(this->rounds - 1, 0);
					this->fired = 1.f;
					mine->Set_eff(ef_fire, pos_t, vec_t, 0.1f / 0.1f);//ノーマル

					mine_v->get_audio().fire.vol(128);
					mine_v->get_audio().fire.play_3D(mine_v->get_move().pos, 250.f);
				}
				this->loadcnt = std::max(this->loadcnt - 1.f / FPS, 0.f);
				this->fired = std::max(this->fired - 1.f / FPS, 0.f);

				//弾の処理
				for (auto& a : this->bullet) { a.UpDate(mine, chara, mine_v, vehicle, hit_obj_p, hit_b_obj_p, MAPPTs); }
			}
			/*弾道描画*/
			void Draw_ammo(void) noexcept {
				for (auto& a : this->bullet) { a.Draw(); }
			}
			/* UI向けにヒット部分を表示*/
			void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
				for (auto& a : this->bullet) {
					if (a.hit_alpha >= 10.f / 255.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * a.hit_alpha));
						hit_Graph.DrawRotaGraph(a.hit_window_x, a.hit_window_y, a.hit_alpha * 0.5f, 0.f, true);//(ismine ? 1.f : 0.5f)
					}
				}
			}

			Guns() {
			}
			Guns(const Guns&) {
			}
		};										/**/
		//履帯BOX2D
		class FootWorld {
		public:
			std::shared_ptr<b2World> world{ nullptr };		/*足world*/
			b2RevoluteJointDef f_jointDef;					/*ジョイント*/
			std::vector<b2Pats> Foot, Wheel, Yudo;			/**/
		};
	public:
		Vehcs* use_veh;																/*固有値*/
	private:
		std::vector<MV1_COLL_RESULT_POLY> hitres;									/*確保*/
		std::vector<pair_hit> hitssort;												/*フレームに当たった順番*/

		b2Pats b2mine;																/*box2d*/
		float spd_buf = 0.f;														/*box2d*/
		bool hit_check = false;														//当たり判定を取るかチェック

		std::vector<Guns> Gun_;														/**/
		float speed_add = 0.f, speed_sub = 0.f;										/**/
		float xrad = 0.f, xradadd = 0.f, xradadd_left = 0.f, xradadd_right = 0.f;	/**/
		float yrad = 0.f, yradadd = 0.f, yradadd_left = 0.f, yradadd_right = 0.f;	/**/
		float zrad = 0.f, zradadd = 0.f, zradadd_left = 0.f, zradadd_right = 0.f;	/**/
		float wheel_Left = 0.f, wheel_Right = 0.f;									//転輪回転
		float wheel_Leftadd = 0.f, wheel_Rightadd = 0.f;							//転輪回転

		std::array<FootWorld, 2> foot; /*足*/
		float per_sus{ 0.f };
		float view_xrad = 0.f;
		float view_yrad = 0.f;
		float xrad_shot = 0.f; //射撃反動x
		float zrad_shot = 0.f; //射撃反動z
		VECTOR_ref wheel_normal = VGet(0.f, 1.f, 0.f);
		std::array<bool, 15> key{ false };	//キー
		bool nearhit{ false };							//

		float body_yrad{ 0.f };							//胴体角度

		Audios_tanks audio;

		std::vector<foot_frame> wheelframe;						/**/
		std::array<std::vector<cat_frame>, 2> b2downsideframe; /*履帯*/
	public:
		float spd_rec;

		using PLAYER_COMMON::PLAYER_COMMON;

		void sort_Hit() {
			std::sort(this->hitssort.begin(), this->hitssort.end(), [](const pair_hit& x, const pair_hit& y) { return x.second < y.second; });
		}

		auto& get_hitssort() noexcept { return this->hitssort; }
		auto& get_hitres() noexcept { return this->hitres; }
		bool HitCheck(int m,moves ray) {
			this->hitres[m] = this->col_line(ray.repos, ray.pos, m);
			if (this->hitres[m].HitFlag == TRUE) {
				this->hitssort[m].first = m;
				this->hitssort[m].second = (ray.repos - this->hitres[m].HitPosition).size();
				return true;
			}
			else {
				this->hitssort[m].first = m;
				this->hitssort[m].second = (std::numeric_limits<float>::max)();
				return false;
			}
		}

		const auto& get_audio() const noexcept { return this->audio; }																	//audio(メニューのみ)
		const float& get_body_yrad(void) const noexcept { return this->body_yrad; }														//ミニマップ用

		/*カメラ指定*/
		void Set_cam(cam_info& camera_main, const VECTOR_ref& eyevec, const float fov_) noexcept {
			float range = 7.5f;
			VECTOR_ref eyepos = this->move.pos + (this->move.mat.yvec() * 3.f) + eyevec * range;
			VECTOR_ref eyetgt = eyepos + eyevec * (-range);

			if (MAPPTs->map_col_nearest(eyepos, &eyetgt)) {
				eyepos = eyetgt;
			}

			camera_main.set_cam_pos(eyepos, eyepos + eyevec * -1.f, this->move.mat.yvec());
			camera_main.near_ = 0.1f;
			easing_set(&camera_main.fov, fov_, 0.9f);
		}
		void reset() {
			this->obj_body.Dispose();
			this->obj_col.Dispose();
			this->hit_check = false;
			this->HP = 0;
			this->HP_parts.clear();
			this->speed_add = 0.f;
			this->speed_sub = 0.f;
			this->speed = 0.f;
			this->move.vec.clear();
			this->hitres.clear();
			this->wheel_Left = 0.f;
			this->wheel_Right = 0.f;
			this->wheel_Leftadd = 0.f;
			this->wheel_Rightadd = 0.f;
			this->xrad = 0.f;
			this->xradadd = 0.f;
			this->xradadd_left = 0.f;
			this->xradadd_right = 0.f;
			this->yrad = 0.f;
			this->yradadd = 0.f;
			this->yradadd_left = 0.f;
			this->yradadd_right = 0.f;
			this->zrad = 0.f;
			this->zradadd = 0.f;
			this->zradadd_left = 0.f;
			this->zradadd_right = 0.f;
			for (auto& g : this->Gun_) { g.Clear(); }
			this->Gun_.clear();
			this->hitssort.clear();
			for (auto& f : foot) {
				f.world.reset();
			}
		}
		static b2Body* CreateBody(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) {
			b2BodyDef f_bodyDef;
			f_bodyDef.type = type;
			f_bodyDef.position.Set(x_, y_);
			f_bodyDef.angle = angle;
			return world->CreateBody(&f_bodyDef);
		}
		void Set(Vehcs* tgt, const EffekseerEffectHandle& gndsmkHndle) {
			//初期位置
			this->move.pos = VGet(1.f, 10.f, 0);
			this->move.mat = MATRIX_ref::RotY(deg2rad(0.f));
			//
			this->use_veh = tgt;
			this->obj_body = this->use_veh->Get_obj().Duplicate();
			this->obj_col = this->use_veh->Get_col().Duplicate();
			this->obj_body.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			//戦車物理set
			VECTOR_ref minmaxsub = this->use_veh->Get_maxpos() - this->use_veh->Get_minpos();
			VECTOR_ref minmaxadd = this->use_veh->Get_minpos() + this->use_veh->Get_maxpos();
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	/*ダイナミックボディに別のボックスシェイプを定義します。*/
			this->b2mine.Set(CreateBody(MAPPTs->world, b2_dynamicBody, this->move.pos.x(), this->move.pos.z(), atan2f(-this->move.mat.zvec().x(), -this->move.mat.zvec().z())), &dynamicBox);	/*シェイプをボディに追加*/
			for (auto& f : foot) {
				f.world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
				float LorR = ((&f == &foot.front()) ? 1.f : -1.f);
				//履帯
				f.Foot.clear();
				{
					b2BodyDef bd;
					b2Body* BodyB = f.world->CreateBody(&bd);
					b2EdgeShape shape;
					shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
					BodyB->CreateFixture(&shape, 0.0f);
					b2Body* BodyA = BodyB;
					b2Vec2 anchor;
					for (const auto& w : this->use_veh->Get_b2upsideframe(&f - &foot.front())) {
						anchor = b2Vec2(w.second.z(), w.second.y());
						if (w.second.x() * LorR > 0) {
							f.Foot.resize(f.Foot.size() + 1);
							b2PolygonShape f_dynamicBox; /*ダイナミックボディに別のボックスシェイプを定義します。*/
							f_dynamicBox.SetAsBox(0.2f, 0.05f);
							f.Foot.back().Set(CreateBody(f.world, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
							f.f_jointDef.Initialize(BodyA, f.Foot.back().Get(), anchor);
							f.world->CreateJoint(&f.f_jointDef);
							BodyA = f.Foot.back().Get();
						}
					}
					if (!f.Foot.empty()) {
						f.f_jointDef.Initialize(BodyA, BodyB, anchor);
						f.world->CreateJoint(&f.f_jointDef);
					}
				}
				for (const auto& w : this->use_veh->Get_b2downsideframe()) {
					auto& w2 = this->b2downsideframe[&w - &this->use_veh->Get_b2downsideframe().front()];
					w2.resize(w.size());
					for (auto& t : w2) {
						t.frame = w[&t - &w2.front()];
						t.gndsmkeffcs.set_loop(gndsmkHndle);
						t.gndsmksize = 0.1f;
					}
				}
				//転輪
				f.Wheel.clear();
				f.Yudo.clear();
				if (!f.Foot.empty()) {
					//転輪(動く)
					for (const auto& w : this->use_veh->Get_wheelframe()) {
						this->wheelframe.resize(this->wheelframe.size() + 1);
						this->wheelframe.back().frame = w;
						VECTOR_ref vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
						if (vects.x() * LorR > 0) {
							f.Wheel.resize(f.Wheel.size() + 1);
							b2CircleShape shape;
							shape.m_radius = vects.y() - 0.1f;
							f.Wheel.back().Set(CreateBody(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
						}
					}
					//誘導輪(動かない)
					for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
						VECTOR_ref vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
						if (vects.x() * LorR > 0) {
							f.Yudo.resize(f.Yudo.size() + 1);
							b2CircleShape shape;
							shape.m_radius = 0.05f;
							f.Yudo.back().Set(CreateBody(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
						}
					}
				}
			}
			//コリジョン
			for (int j = 0; j < this->obj_col.mesh_num(); j++) {
				this->obj_col.SetupCollInfo(8, 8, 8, -1, j);
			}
			this->hitres.resize(this->obj_col.mesh_num());		//モジュールごとの当たり判定結果を確保
			this->hitssort.resize(this->obj_col.mesh_num());	//モジュールごとの当たり判定順序を確保
			//砲
			this->Gun_.resize(this->use_veh->Get_gunframe().size());
			for (auto& g : this->Gun_) { g.Set(this->use_veh->Get_gunframe(&g - &this->Gun_.front())); }
			this->HP_full = this->use_veh->Get_HP();
			//ヒットポイント
			this->HP = this->HP_full;
			this->HP_parts.resize(this->obj_col.mesh_num());
			for (auto& h : this->HP_parts) { h = this->HP_full; }//モジュール耐久

			this->audio.Duplicate(this->use_veh->audio);	//audio
		}
		static void BodyFrameLocalMatrix(MV1& obj_body_t, const frames& id, const MATRIX_ref& mat_t = MGetIdent()) noexcept { obj_body_t.SetFrameLocalMatrix(id.first, mat_t * MATRIX_ref::Mtrans(id.second)); }
		template<class Map>
		void UpDate(const cam_info& cams, const std::shared_ptr<Chara>& mine, const std::vector<std::shared_ptr<Chara>>& chara, const std::vector<std::shared_ptr<vehicles>>& vehicle, HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p, std::shared_ptr<Map>& MAPPTs) {
			if (vehicle[0] == shared_from_this()) {
				key[0] = (CheckHitKey(KEY_INPUT_SPACE) != 0);//((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);   //射撃
				key[1] = ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0); //マシンガン
				key[2] = (CheckHitKey(KEY_INPUT_W) != 0);				//前進
				key[3] = (CheckHitKey(KEY_INPUT_S) != 0);				//後退
				key[4] = (CheckHitKey(KEY_INPUT_D) != 0);				//右
				key[5] = (CheckHitKey(KEY_INPUT_A) != 0);				//左
				{
					if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) { //砲塔ロック
						view_yrad = 0.f;
						view_xrad = 0.f;
					}
					else {
						//狙い
						VECTOR_ref vec_a;
						{
							VECTOR_ref endpos = cams.campos + (cams.camvec - cams.campos).Norm() * (100.f);
							//マップに当たったか
							//mapparts->map_col_line_nearest(campos, &endpos);

							vec_a = (this->obj_body.frame(this->Gun_[0].Getgun_info().get_frame2().first) - endpos).Norm();
						}
						//反映
						auto vec_z = this->obj_body.frame(this->Gun_[0].Getgun_info().get_frame3().first) - this->obj_body.frame(this->Gun_[0].Getgun_info().get_frame2().first);
						float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
						float a_hyp = std::hypotf(vec_a.x(), vec_a.z());
						float cost = (vec_a.z() * vec_z.x() - vec_a.x() * vec_z.z()) / (a_hyp * z_hyp);

						view_yrad = (atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / 5.f; //cos取得2D
						view_xrad = (atan2f(-vec_z.y(), z_hyp) - atan2f(vec_a.y(), a_hyp)) / 5.f;
					}
				}
			}
			else {
				view_yrad = 0.f;
				view_xrad = 0.f;
			}
			//戦車演算
			{
				auto yrad_p = 0.f;
				spd_rec = this->speed;
				this->nearhit = false;
				//砲塔旋回
				{
					//角度指示
					for (auto& g : this->Gun_) {
						g.SetGunRad(view_xrad, view_yrad, this->use_veh->Get_turret_rad_limit() / FPS);
					}
					//反映
					for (auto& g : this->Gun_) {
						if (g.Getgun_info().get_frame1().first > 0) {
							BodyFrameLocalMatrix(this->obj_body, g.Getgun_info().get_frame1(), MATRIX_ref::RotY(g.yrad));
							BodyFrameLocalMatrix(this->obj_col, g.Getgun_info().get_frame1(), MATRIX_ref::RotY(g.yrad));
						}
						if (g.Getgun_info().get_frame2().first > 0) {
							BodyFrameLocalMatrix(this->obj_body, g.Getgun_info().get_frame2(), MATRIX_ref::RotX(g.xrad));
							BodyFrameLocalMatrix(this->obj_col, g.Getgun_info().get_frame2(), MATRIX_ref::RotX(g.xrad));
						}
						if (g.Getgun_info().get_frame3().first > 0) {
							BodyFrameLocalMatrix(this->obj_body, g.Getgun_info().get_frame3(), MATRIX_ref::Mtrans(VGet(0.f, 0.f, g.Getfired() * 0.5f)));
							BodyFrameLocalMatrix(this->obj_col, g.Getgun_info().get_frame3(), MATRIX_ref::Mtrans(VGet(0.f, 0.f, g.Getfired() * 0.5f)));
						}
					}
				}
				//転輪
				{
					auto y_vec = this->move.mat.yvec();
					for (auto& f : this->wheelframe) {
						MATRIX_ref tmp;
						this->obj_body.frame_reset(f.frame.first);
						auto startpos = this->obj_body.frame(f.frame.first);
						auto hp2 = MAPPTs->map_col_line(startpos + y_vec * ((-f.frame.second.y()) + 2.f), startpos + y_vec * ((-f.frame.second.y()) - 0.3f));

						easing_set(&f.will_y, (hp2.HitFlag == TRUE) ? (hp2.HitPosition.y + y_vec.y() * f.frame.second.y() - startpos.y()) : -0.3f, 0.9f);
						tmp = MATRIX_ref::Mtrans(VGet(0.f, f.will_y, 0.f));

						this->obj_body.SetFrameLocalMatrix(f.frame.first, MATRIX_ref::RotX((f.frame.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * tmp * MATRIX_ref::Mtrans(f.frame.second));
					}
					for (const auto& f : this->use_veh->Get_wheelframe_nospring()) {
						this->obj_body.SetFrameLocalMatrix(f.first, MATRIX_ref::RotX((f.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * MATRIX_ref::Mtrans(f.second));
					}
				}
				easing_set(
					&wheel_normal,
					((this->obj_body.frame(this->use_veh->Get_square(0)) - this->obj_body.frame(this->use_veh->Get_square(3))).cross(this->obj_body.frame(this->use_veh->Get_square(1)) - this->obj_body.frame(this->use_veh->Get_square(2)))).Norm(),
					0.95f);
				//履帯
				{
					for (auto& g : this->b2downsideframe) {
						for (auto& t : g) {
							MATRIX_ref tmp;
							this->obj_body.frame_reset(t.frame.first);
							auto startpos = this->obj_body.frame(t.frame.first);
							t.colres = MAPPTs->map_col_line(startpos + this->move.mat.yvec() * ((-t.frame.second.y()) + 2.f), startpos + this->move.mat.yvec() * ((-t.frame.second.y()) - 0.3f));
							if (t.colres.HitFlag == TRUE) {
								tmp = MATRIX_ref::Mtrans(VGet(0.f, t.colres.HitPosition.y + this->move.mat.yvec().y() * t.frame.second.y() - startpos.y(), 0.f));
							}
							else {
								tmp = MATRIX_ref::Mtrans(VGet(0.f, -0.4f, 0.f));
							}
							this->obj_body.SetFrameLocalMatrix(t.frame.first, tmp * MATRIX_ref::Mtrans(t.frame.second));
						}
					}
				}
				//移動
				auto isfloat = (this->move.pos.y() == -this->use_veh->Get_down_in_water());
				//行列
				{
					//リセット
					{
						const auto yvec = (this->use_veh->Get_isfloat() && isfloat) ? VGet(0.f, 1.f, 0.f) : wheel_normal;
						auto pp = (this->move.mat * MATRIX_ref::RotVec2(VGet(0.f, 1.f, 0.f), yvec).Inverse()).zvec() * -1.f;

						yrad_p = atan2f(pp.x(), pp.z());
						//printfDx("%5.2f\n", rad2deg(yrad_p));

						const auto zvec = MATRIX_ref::Vtrans(VGet(-sinf(yrad_p), 0.f, -cosf(yrad_p)), MATRIX_ref::RotVec2(VGet(0.f, 1.f, 0.f), yvec));
						this->move.mat = MATRIX_ref::Axis1_YZ(yvec, zvec);
					}
					//地面判定
					{
						auto yp = this->move.pos.y();
						float hight_t = 0.f;
						int cnt_t = 0;
						bool tatch = false;
						for (const auto& g : this->b2downsideframe) {
							for (const auto& t : g) {
								hight_t += this->obj_body.frame(t.frame.first).y();
								cnt_t++;
								if (t.colres.HitFlag == TRUE) {
									tatch = true;
								}
							}
						}
						//*
						for (const auto& s : this->use_veh->Get_square()) {
							auto p_t = this->obj_body.frame(s);
							auto hp2 = MAPPTs->map_col_line(p_t + VGet(0.f, 2.f, 0.f), p_t - VGet(0.f, 0.3f, 0.f));
							if (hp2.HitFlag == TRUE) {
								hight_t += hp2.HitPosition.y;
								cnt_t++;
								tatch = true;
							}
						}
						//*/
						if (tatch) {
							easing_set(&yp, (hight_t / cnt_t), 0.9f);
						}
						this->move.pos.y(yp);

						//地面or水面にいるかどうか
						if (tatch || (this->use_veh->Get_isfloat() && isfloat)) {
							//前進後退
							{
								const auto old = this->speed_add + this->speed_sub;
								if (key[2]) {
									this->speed_add = (this->speed_add < (this->use_veh->Get_flont_speed_limit() / 3.6f)) ? (this->speed_add + (0.06f / 3.6f) * (60.f / FPS)) : this->speed_add;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.7f / 3.6f) * (60.f / FPS)) : this->speed_sub;
								}
								if (key[3]) {
									this->speed_sub = (this->speed_sub > (this->use_veh->Get_back_speed_limit() / 3.6f)) ? (this->speed_sub - (0.06f / 3.6f) * (60.f / FPS)) : this->speed_sub;
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.7f / 3.6f) * (60.f / FPS)) : this->speed_add;
								}
								if (!(key[2]) && !(key[3])) {
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
								}
								this->speed = (old + ((this->speed_add + this->speed_sub) - old) * 0.1f) / FPS;
								this->move.vec = this->move.mat.zvec() * -this->speed;
							}
							//旋回
							{
								this->yradadd_left = (key[4]) ? std::max(this->yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->use_veh->Get_body_rad_limit())) : std::min(this->yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd_right = (key[5]) ? std::min(this->yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->use_veh->Get_body_rad_limit())) : std::max(this->yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd = (this->yradadd_left + this->yradadd_right) / FPS;
							}
							//慣性
							{
								const auto xradold = this->xradadd;
								this->xradadd = deg2rad(-((this->speed) / (60.f / FPS)) / (0.1f / 3.6f) * 50.f);
								easing_set(&this->xrad, std::clamp(this->xradadd - xradold, deg2rad(-30.f), deg2rad(30.f)), 0.995f);

								auto zradold = this->zradadd;
								this->zradadd = deg2rad(-this->yradadd / (deg2rad(5.f) / FPS) * 30.f);
								easing_set(&this->zrad, std::clamp(this->zradadd - zradold, deg2rad(-15.f), deg2rad(15.f)), 0.995f);

								this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.xvec(), -this->xrad) * MATRIX_ref::RotAxis(this->move.mat.zvec(), this->zrad);
							}
							//
						}
						else {
							this->move.vec.yadd(M_GR / powf(FPS, 2.f));
						}
					}


					this->move.pos += this->move.vec;
					//射撃反動
					{
						easing_set(&xrad_shot, deg2rad(-this->Gun_[0].Getfired() * this->Gun_[0].Getcaliber(0) * 50.f) * cos(this->Gun_[0].yrad), 0.85f);
						easing_set(&zrad_shot, deg2rad(-this->Gun_[0].Getfired() * this->Gun_[0].Getcaliber(0) * 50.f) * sin(this->Gun_[0].yrad), 0.85f);
						this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.xvec(), -xrad_shot) * MATRIX_ref::RotAxis(this->move.mat.zvec(), zrad_shot);
					}
				}
				//浮く
				if (this->use_veh->Get_isfloat()) {
					this->move.pos.y(std::max(this->move.pos.y(), -this->use_veh->Get_down_in_water()));
				}
				//転輪
				this->wheel_Leftadd = -(this->speed * 2.f - this->yradadd * 5.f);
				this->wheel_Rightadd = -(this->speed * 2.f + this->yradadd * 5.f);

				this->wheel_Left += this->wheel_Leftadd;  // -this->yradadd * 5.f;
				this->wheel_Right += this->wheel_Rightadd; // +this->yradadd * 5.f;

				//射撃
				{
					for (int i = 0; i < this->Gun_.size(); i++) {
						auto& cg = this->Gun_[i];
						cg.UpDate(
							key[(i == 0) ? 0 : 1],
							this->obj_body.frame(cg.Getgun_info().get_frame3().first),
							(this->obj_body.frame(cg.Getgun_info().get_frame3().first) - this->obj_body.frame(cg.Getgun_info().get_frame2().first)).Norm(), mine, chara, shared_from_this(), vehicle, hit_obj_p, hit_b_obj_p, MAPPTs);
					}
				}
				//戦車壁判定
				this->b2mine.UpDate(this->move.vec, this->yradadd);
			}
		}
		void UpDate_after() {
			//戦車演算
			{
				{
					//戦車座標反映
					auto pp = this->move.mat.zvec();
					this->move.mat *= MATRIX_ref::RotY(-this->b2mine.Rad() - atan2f(-pp.x(), -pp.z()));
					this->move.pos.x(this->b2mine.Pos().x);
					this->move.pos.z(this->b2mine.Pos().y);
					float spdrec = this->spd_buf;
					easing_set(&this->spd_buf, this->b2mine.Speed() * ((this->spd_buf > 0) ? 1.f : -1.f), 0.99f);
					this->speed = this->spd_buf - spdrec;
					//this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.yvec(),this->yradadd);

					//転輪
					b2Vec2 tmpb2 = b2Vec2((M_GR / FPS * 0.5f) * (this->move.mat.zvec().dot(VGet(0, 1.f, 0))), (M_GR / FPS * 0.5f) * (this->move.mat.yvec().dot(VGet(0, 1.f, 0))));
					int LorR = 0;
					for (auto& f : foot) {
						size_t i = 0;
						VECTOR_ref vects;
						int LR_t = ((LorR == 0) ? 1 : -1);
						if (f.Foot.size() != 0) {
							i = 0;
							for (const auto& w : this->use_veh->Get_wheelframe()) {
								vects = VECTOR_ref(VTransform(VGet(0, 0, 0), MV1GetFrameLocalMatrix(this->obj_body.get(), w.first)));
								if (vects.x() * LR_t > 0) {
									f.Wheel[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
								}
							}
							i = 0;
							for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
								vects = VTransform(VGet(0, 0, 0), MV1GetFrameLocalMatrix(this->obj_body.get(), w.first));
								if (vects.x() * LR_t > 0) {
									f.Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
								}
							}
							for (auto& t : f.Foot) {
								t.SetLinearVelocity(tmpb2); //
							}

							f.world->Step(0.1f, 3, 3);

							for (auto& t : f.Foot) {
								t.pos = VGet(t.pos.x(), t.Pos().y, t.Pos().x);
							}
						}
						{
							i = 0;
							for (const auto& w : this->use_veh->Get_b2upsideframe(LorR)) {
								if (w.second.x() * LR_t > 0) {
									auto& t = f.Foot[i++];
									t.pos = VGet(w.second.x(), t.pos.y(), t.pos.z());
									this->obj_body.SetFrameLocalMatrix(w.first, MATRIX_ref::Mtrans(t.pos));
								}
							}
						}
						LorR++;
					}
				}

				this->obj_body.SetMatrix(this->move.mat * MATRIX_ref::Mtrans(this->move.pos));
				this->obj_col.SetMatrix(this->obj_body.GetMatrix());

				for (auto& g : this->b2downsideframe) {
					for (auto& t : g) {
						if (t.colres.HitFlag == TRUE) {
							easing_set(&t.gndsmksize, 0.1f + std::abs(this->speed - spd_rec) / ((0.01f / 3.6f) / FPS) * 1.f, 0.95f);
						}
						else {
							t.gndsmksize = 0.1f;
						}
						t.gndsmkeffcs.handle.SetPos(this->obj_body.frame(t.frame.first) + this->move.mat.yvec() * (-t.frame.second.y()));
						t.gndsmkeffcs.handle.SetScale(t.gndsmksize);
					}
				}
			}
			//エンジン音
			if (!this->get_audio().engine.check()) {
				this->get_audio().engine.vol(64);
				this->get_audio().engine.play_3D(this->move.pos, 50.f);
			}
		}
		//判定起動
		const bool set_ref_col(VECTOR_ref& StartPos, VECTOR_ref& EndPos) {
			//すでに起動しているなら無視
			if (this->nearhit) {
				return true;
			}
			if (Segment_Point_MinLen(StartPos, EndPos, this->move.pos) <= 10.0f) {
				//判定起動
				this->nearhit = true;
				for (int i = 0; i < this->obj_col.mesh_num(); i++) {
					this->obj_col.RefreshCollInfo(-1, i);
				}
				return true;
			}
			return false;
		}
		void Dispose() {
			this->b2mine.Dispose();			/*Box2D*/
			for (auto& t : foot) {
				for (auto& f : t.Foot) {
					f.Dispose();
				}
				t.Foot.clear();
				for (auto& f : t.Wheel) {
					f.Dispose();
				}
				t.Wheel.clear();
				for (auto& f : t.Yudo) {
					f.Dispose();
				}
				t.Yudo.clear();
			}
			for (auto& g : this->b2downsideframe) {
				for (auto& t : g) {
					t.gndsmkeffcs.handle.Dispose();
				}
				g.clear();
			}
			this->reset();
			this->audio.Dispose();
		}
		void Draw() {
			MV1SetFrameTextureAddressTransform(this->obj_body.get(), 0, -this->wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
			this->obj_body.DrawMesh(0);
			MV1SetFrameTextureAddressTransform(this->obj_body.get(), 0, -this->wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
			this->obj_body.DrawMesh(1);
			MV1ResetFrameTextureAddressTransform(this->obj_body.get(), 0);
			for (int i = 2; i < this->obj_body.mesh_num(); i++) {
				this->obj_body.DrawMesh(i);
			}
			//obj_col.DrawModel();
		}
		void Draw_ammo() {
			for (auto& cg : this->Gun_) {
				cg.Draw_ammo();
			}
		}
		//UI描画用用意
		void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
			for (auto& cg : this->Gun_) {
				cg.Draw_Hit_UI(hit_Graph);
			}
		}
	};
};