#pragma once
#define FRAME_RATE 90.f
//リサイズ
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)
// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.15f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離
//
enum Effect {
	ef_fire, //発砲炎
	ef_reco, //小口径跳弾
	ef_smoke, //銃の軌跡
	ef_gndsmoke,//地面の軌跡
	ef_fire2, //発砲炎
	effects, //読み込む
};
enum EnumGunParts {
	PARTS_NONE,
	PARTS_BASE,
	PARTS_MAGAZINE,
	PARTS_MAZZULE,
	PARTS_GRIP,
	PARTS_UPER_HGUARD,
	PARTS_UNDER_HGUARD,
	PARTS_DUSTCOVER,
	PARTS_STOCK,
	PARTS_LIGHT,
	PARTS_LASER,
	PARTS_MOUNT,
	PARTS_SIGHT,
	PARTS_FOREGRIP,
	PARTS_NUM,
};
enum EnumAttachPoint {
	UPER,
	UNDER,
	LEFTSIDE,
	RIGHTSIDE,
	SIDEMOUNT,
	SIDEMOUNT_BASE,
	STOCK_BASE,
	DUSTCOVER_BASE,
	UPER_HANDGUARD,
	UNDER_HANDGUARD,
	MAZZULE_,
	GRIP_BASE,
	NUM_,
};
//option
class OPTION {
public:
	bool DoF = false;
	bool bloom = false;
	bool shadow = false;
	bool useVR = true;
	float fov_pc = 45.f;
	bool vsync = false;
	OPTION() {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		DoF = getparams::_bool(mdata);
		bloom = getparams::_bool(mdata);
		shadow = getparams::_bool(mdata);
		useVR = getparams::_bool(mdata);
		fov_pc = getparams::_float(mdata);
		vsync = getparams::_bool(mdata);
		FileRead_close(mdata);
		SetOutApplicationLogValidFlag(TRUE);
	}
	~OPTION() {
	}
};
//
class Mainclass {
private:
	//銃用オーディオ
	class Audios {
	private:
	public:
		SoundHandle shot;
		SoundHandle slide;
		SoundHandle trigger;
		SoundHandle mag_down;
		SoundHandle mag_Set;

		SoundHandle case_down;

		SoundHandle load_;
		SoundHandle sort_;

		void Set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_Set = SoundHandle::Load("data/audio/mag_Set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_2.wav");
			load_ = SoundHandle::Load("data/audio/load.wav");
			sort_ = SoundHandle::Load("data/audio/sort.wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);

		}

		void Duplicate(Audios& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->shot = tgt.shot.Duplicate();
			this->slide = tgt.slide.Duplicate();
			this->trigger = tgt.trigger.Duplicate();
			this->mag_down = tgt.mag_down.Duplicate();
			this->mag_Set = tgt.mag_Set.Duplicate();
			this->load_ = tgt.load_.Duplicate();
			this->sort_ = tgt.sort_.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}

		void Dispose() {
			this->shot.Dispose();
			this->slide.Dispose();
			this->trigger.Dispose();
			this->mag_down.Dispose();
			this->mag_Set.Dispose();
			this->load_.Dispose();
			this->sort_.Dispose();
		}
	};
	//銃、マガジン共通モデル
	class Models {
	private:
	public:
		std::string name;
		std::string path;
		MV1 model;
		void Ready(std::string path_, std::string named) {
			name = named;
			path = path_ + named;
			MV1::Load(path + "/model.mv1", &model, true);
		}
		void Set() {
		}
	};
	//弾データ
	class Ammo_info {
	private:
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		int damage = 10;//ダメージ

		MV1 ammo;
		void Set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_int(mdata);//ダメージ
			FileRead_close(mdata);

			MV1::Load("data/ammo/" + this->name + "/ammo.mv1", &ammo, true);
		}
	};
protected:
	class Chara;
	class Items;
	class Meds;
	class gunparts;
	//命中根
	struct Hit {			/**/
		bool flug = false;	/*弾痕フラグ*/
		int use = 0;		/*使用フレーム*/
		MV1 pic;			/*弾痕モデル*/
		VECTOR_ref pos;		/*座標*/
		MATRIX_ref mat;		/**/
		void Set(const MV1&hit_pic) {
			this->flug = false;
			this->use = 0;
			this->pic = hit_pic.Duplicate();
			this->mat.clear();
			this->pos.clear();
		}
		void Start(const VECTOR_ref& pos_t,const MATRIX_ref& mat_t) {
			this->flug = true;
			this->use = 0;
			this->pic.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
			this->mat = mat_t;
			this->pos = pos_t;
		}
		void Draw() {
			if (this->flug) {
				this->pic.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pic.DrawFrame(this->use);
			}
		}
	};
	//実際に発射される弾
	class Ammos {
	private:
	public:
		bool hit = false;
		float hit_count = 0.f;
		float hit_r = 0.f;
		int hit_x = 0, hit_y = 0;
		bool flug = false;
		float cnt = 0.f;
		unsigned int color = 0;
		Ammo_info* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos;
		VECTOR_ref vec;
		void Ready() {
			this->flug = false;
		}
		void Set(Ammo_info* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->color = GetColor(255, 255, 172);
			this->spec = spec_t;
			this->yadd = 0.f;

			this->pos = pos_t;
			this->repos = this->pos;
			this->vec = vec_t;
		}
		template<class Y, class D>
		void update(Chara* c, std::vector<Chara>* chara, std::unique_ptr<Y, D>& MAPPTs, std::vector<Hit>& hit_obj, size_t& hitbuf) {
			if (this->flug) {
				this->repos = this->pos;
				this->pos += this->vec * (this->spec->speed / GetFPS());
				//判定
				{
					auto p = MAPPTs->map_col_line(this->repos, this->pos);
					if (p.HitFlag) {
						this->pos = p.HitPosition;
					}
					//*
					for (auto& tgt : *chara) {
						if (tgt.HP == 0) {
							continue;
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 0);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage * 3 / 5, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 50;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 1);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage * 3, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 70;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 2);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 100;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
					}
					if (p.HitFlag && this->flug) {
						this->flug = false;
						c->effcs_gndhit[c->use_effcsgndhit].set(this->pos, p.Normal, 0.025f / 0.1f);
						++c->use_effcsgndhit %= c->effcs_gndhit.size();

						//弾痕のセット
						{
							float asize = this->spec->caliber*100.f*2.f;
							VECTOR_ref y_vec = p.Normal;
							VECTOR_ref z_vec = y_vec.cross(this->vec.Norm()).Norm();
							VECTOR_ref scale = VGet(asize / std::abs(this->vec.Norm().dot(y_vec)), asize, asize);

							hit_obj[hitbuf].Start(this->pos + y_vec * 0.02f, MATRIX_ref::Scale(scale)*  MATRIX_ref::Axis1(y_vec.cross(z_vec), y_vec, z_vec));
							++hitbuf %= hit_obj.size();
						}
					}
					//*/
				}
				//消す(3秒たった、スピードが0以下、貫通が0以下)
				if (this->cnt >= 3.f || this->spec->speed < 0.f || this->spec->pene <= 0.f) {
					this->flug = false;
				}
				//
			}
		}
		void Set_Draw() {
			if (this->hit) {
				this->hit_r = 2.f;
				this->hit_count = 0.5f;
				VECTOR_ref p = ConvWorldPosToScreenPos((this->pos).get());
				if (p.z() >= 0.f&&p.z() <= 1.f) {
					this->hit_x = int(p.x());
					this->hit_y = int(p.y());
				}
				this->hit = false;
			}
			if (this->hit_count != 0.f) {
				easing_set(&this->hit_r, 1.f, 0.7f);
			}
			else {
				easing_set(&this->hit_r, 0.f, 0.8f);
			}
			this->hit_count = std::clamp(this->hit_count - 1.f / GetFPS(), 0.f, 1.f);
		}
		void Draw() {
			if (this->flug) {
				DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->caliber - 0.00762f) * 0.1f + 0.00762f), this->color, GetColor(255, 255, 255));
			}
		}
	};
	//パフォーマンス
	struct performance {
		float recoil = 0.f;
		float weight = 0.f;

		void operator+=(performance& t) {
			this->recoil += t.recoil;
			this->weight += t.weight;
		}
		void operator-=(performance& t) {
			this->recoil -= t.recoil;
			this->weight -= t.weight;
		}
	};
	//パーツデータ
	class gunparts {
	private:
	public:
		size_t id = 0;
		Models mod;
		performance per;
		//parts
		size_t mazzule_id = 0;
		GraphHandle reticle;
		std::vector<std::string> can_attach;
		//base
		class fs {
		public:
			frames magazine_f;
			frames magazine2_f;
			frames mazzule_f;
			frames left_f;
			frames site_f;
			frames right_f;

			frames cate_f;

			void Reset() {
				magazine_f.first = INT_MAX;
				magazine2_f.first = INT_MAX;
				mazzule_f.first = INT_MAX;
				left_f.first = INT_MAX;
				site_f.first = INT_MAX;
				right_f.first = INT_MAX;
				cate_f.first = INT_MAX;
			}

			void get_frame(MV1& obj_) {
				for (int i = 0; i < int(obj_.frame_num()); i++) {
					std::string p = obj_.frame_name(i);
					if (p == std::string("mag_fall")) {
						this->magazine_f = { i, obj_.frame(i) };//マガジン
						this->magazine2_f = { i + 1, obj_.frame(i + 1) };//マガジン
					}
					else if (p == std::string("mazzule")) {
						this->mazzule_f = { i, obj_.frame(i) };//マズル
					}
					else if (p == std::string("LEFT")) {
						this->left_f = { i, obj_.frame(i) };//左手
					}
					else if (p == std::string("site")) {
						this->site_f = { i, obj_.frame(i) };//アイアンサイト
					}
					else if (p == std::string("RIGHT")) {
						this->right_f = { i, obj_.frame(i) };//左手
					}
					else if (p == std::string("cate")) {
						this->cate_f = { i, obj_.frame(i) };//左手
					}
				}
			}

		};
		fs frame_s;
		std::vector <uint8_t> select;//セレクター
		Audios audio;
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//再装填時間
		float ammo_speed = 100.f;//弾速
		float ammo_pene = 10.f;//貫通
		float ammo_damege = 10.f;//ダメージ
		std::vector<Ammo_info> ammo;
		//gunparts* magazine_ptr = nullptr;
		//mag
		size_t cap = 1;
		//普通
		void Set_data(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			//
			//テキスト
			{
				int mdata = FileRead_open((this->mod.path + "/data.txt").c_str(), FALSE);
				//
				{
					per.recoil = getparams::_float(mdata);
					per.weight = getparams::_float(mdata);
				}
				//
				{
					can_attach.clear();
					while (FileRead_eof(mdata) == 0) {
						can_attach.resize(can_attach.size() + 1);
						can_attach.back() = getparams::get_str(mdata);
					}
				}
				//
				FileRead_close(mdata);
			}
			//
		}
		//
		void Set_data_mazzule(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			//
			//テキスト
			{
				int mdata = FileRead_open((this->mod.path + "/data.txt").c_str(), FALSE);
				//
				{
					per.recoil = getparams::_float(mdata);
					per.weight = getparams::_float(mdata);
					mazzule_id = getparams::_ulong(mdata);
				}
				//
				{
					can_attach.clear();
					while (FileRead_eof(mdata) == 0) {
						can_attach.resize(can_attach.size() + 1);
						can_attach.back() = getparams::get_str(mdata);
					}
				}
				//
				FileRead_close(mdata);
			}
			//
		}
		void Set_data_sight(const size_t& id_) {
			Set_data(id_);
			//
			SetUseASyncLoadFlag(FALSE);
			this->reticle = GraphHandle::Load(this->mod.path + "/reticle.png");
			SetUseASyncLoadFlag(FALSE);
			//
		}
		void Set_data_base(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			//フレーム
			this->frame_s.Reset();
			this->frame_s.get_frame(this->mod.model);
			//テキスト
			{
				int mdata = FileRead_open((this->mod.path + "/data.txt").c_str(), FALSE);
				//セレクター設定
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else if (getparams::getright(p.c_str()).find("semi") != std::string::npos) {
						this->select.emplace_back(uint8_t(1));					//セミオート=1
					}
					else if (getparams::getright(p.c_str()).find("full") != std::string::npos) {
						this->select.emplace_back(uint8_t(2));					//フルオート=2
					}
					else if (getparams::getright(p.c_str()).find("3b") != std::string::npos) {
						this->select.emplace_back(uint8_t(3));					//3連バースト=3
					}
					else if (getparams::getright(p.c_str()).find("2b") != std::string::npos) {
						this->select.emplace_back(uint8_t(4));					//2連バースト=4
					}
					else {
						this->select.emplace_back(uint8_t(1));
					}
				}
				//サウンド
				this->audio.Set(mdata);
				//弾データ
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else {
						this->ammo.resize(this->ammo.size() + 1);
						this->ammo.back().name = p;
					}
				}
				//マガジン
				std::string tmp = getparams::_str(mdata);
				/*
				for (auto& g : mag_data) {
					if (tmp == g.mod.name) {
						this->magazine_ptr = &g;
						break;
					}
				}
				*/
				//反動
				this->recoil_xup = getparams::_float(mdata);
				this->recoil_xdn = getparams::_float(mdata);
				this->recoil_yup = getparams::_float(mdata);
				this->recoil_ydn = getparams::_float(mdata);
				//リロードタイム
				this->reload_time = getparams::_float(mdata);
				//
				{
					per.recoil = getparams::_float(mdata);
					per.weight = getparams::_float(mdata);
				}
				//
				{
					can_attach.clear();
					while (FileRead_eof(mdata) == 0) {
						can_attach.resize(can_attach.size() + 1);
						can_attach.back() = getparams::get_str(mdata);
					}
				}
				//
				FileRead_close(mdata);
			}
			//
			for (auto& a : this->ammo) {
				a.Set();
			}
			//
			this->mod.Set();
			//
		}
		void Set_data_mag(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			{
				int mdata = FileRead_open((this->mod.path + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_long(mdata);//口径
				//弾データ
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else {
						this->ammo.resize(this->ammo.size() + 1);
						this->ammo.back().name = p;
					}
				}
				FileRead_close(mdata);
			}
			this->mod.Set();
		}
	};
	//薬品データ
	class Meds {
	private:
	public:
		size_t id = 0;
		Models mod;
		/**/
		int repair = 0;
		/**/
		void Set_data(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			{
				int mdata = FileRead_open((this->mod.path + "/data.txt").c_str(), FALSE);
				this->repair = getparams::_long(mdata);//口径
				FileRead_close(mdata);
			}
			this->mod.Set();
		}
	};
	//player
	class Chara {
	private:
		class ammo_obj {
		public:
			MV1 second;
			float cnt = -1.f;
			bool down = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
			float rate;

			void Ready() {
				this->cnt = -1.f;
				this->pos = VGet(0, 0, 0);
				this->mat = MGetIdent();
				this->rate = 1.f;
			}
			void Set(Ammo_info* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t, const MATRIX_ref& mat_t, const float& rate_t = 1) {
				this->second.Dispose();
				this->second = spec_t->ammo.Duplicate();
				this->cnt = 0.f;
				this->rate = rate_t;
				this->pos = pos_t;//排莢
				this->add = vec_t * this->rate;//排莢ベクトル
				this->mat = mat_t;
			}
			template<class Y, class D>
			void get(std::unique_ptr<Y, D>& MAPPTs, Chara&c) {
				if (this->cnt >= 0.f) {
					float fps = GetFPS() / this->rate;

					this->cnt += 1.f / fps;
					this->pos += this->add*(float(200 + GetRand(1000)) / 1000.f);
					this->add.yadd(-9.8f / powf(fps, 2.f));

					auto pp = MAPPTs->map_col_line(this->pos + VGet(0, 1.f, 0), this->pos - VGet(0, 0.008f, 0));
					if (pp.HitFlag == 1) {
						this->pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.008f, 0);
						this->add += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(this->add*-1.f)*1.25f);
						easing_set(&this->add, VGet(0, 0, 0), 0.95f);
						if (!this->down) {
							c.audio.case_down.play_3D(c.pos + c.pos_gun, 1.f);
						}

						this->mat *= MATRIX_ref::RotVec2(this->mat.yvec(), VECTOR_ref(pp.Normal));

						this->down = true;
					}
					else {
						this->mat *= MATRIX_ref::RotY(deg2rad(360 * 10 + GetRand(360 * 20)) / fps);
					}
					if (this->cnt >= 3.f) {
						this->cnt = -1.f;
					}

					this->second.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				}
				else {
					this->down = false;
				}
			}
			void Draw() {
				if (this->cnt >= 0.f) {
					this->second.DrawModel();
				}
			}
			void delete_cart() {
				this->second.Dispose();
			}
		};
		struct gun_state {//銃のデータ
			size_t ammo_cnt = 0;		//現在の装弾数カウント
			size_t ammo_total = 0;		//総所持弾数
			std::vector<size_t> mag_in;	//マガジン内データ
			uint8_t select = 0;			//セレクター

			void init() {
				this->ammo_total = 0;
				//this->select = 0;
				this->mag_in.clear();
			}
			//マガジンを1つ追加(装填無し)
			void mag_plus(gunparts* magazine) {
				this->ammo_total += magazine->cap;
				this->mag_in.insert(this->mag_in.end(), magazine->cap);
			}
			//マガジンを1つ追加(装填あり)
			void mag_insert(gunparts* magazine) {
				mag_plus(magazine);
				if (this->mag_in.size() >= 1) {
					this->ammo_cnt = this->mag_in.front();//改良
				}
				else {
					this->ammo_cnt = std::clamp<size_t>(this->ammo_total, 0, magazine->cap);//改良
				}
			}
			//マガジンを落とす
			void mag_release() {
				size_t dnm = this->ammo_cnt - 1;
				//弾数
				if (this->ammo_cnt >= 1) {
					this->ammo_cnt = 1;
				}
				else {
					dnm = 0;
				}
				this->ammo_total -= dnm;
				this->mag_in.erase(this->mag_in.begin());
			}
			//マガジンを落とす
			void mag_release_end() {
				this->mag_in.pop_back();
			}
			//射撃する
			void mag_shot(const bool& reloadf) {
				this->ammo_cnt--;
				this->ammo_total--;
				if (!reloadf && this->mag_in.size() >= 1 && this->mag_in.front() > 0) {
					this->mag_in.front()--;
				}
			}
			//スライドを引いて装填
			void mag_slide() {
				if (this->ammo_cnt <= 1) {
					this->ammo_cnt += this->mag_in.front();
				}
			}
		};
		class fs {
		public:
			//
			frames head_f;
			//
			frames LEFTeye_f;
			frames RIGHTeye_f;
			//
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

			void get_frame(MV1& obj_, float*head_hight) {
				for (int i = 0; i < int(obj_.frame_num()); i++) {
					std::string p = obj_.frame_name(i);
					if (p == std::string("グルーブ")) {
						this->bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("下半身")) {
						this->bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->LEFTfoot1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひざ") != std::string::npos) {
						this->LEFTfoot2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->LEFTreg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首先") != std::string::npos) {
						this->LEFTreg2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->RIGHTfoot1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひざ") != std::string::npos) {
						this->RIGHTfoot2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->RIGHTreg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首先") != std::string::npos) {
						this->RIGHTreg2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
						this->bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身2") != std::string::npos) {
						this->body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
						this->head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
						*head_hight = obj_.frame(this->head_f.first).y();
					}
					else if (p.find("右目先") != std::string::npos) {
						this->RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左目先") != std::string::npos) {
						this->LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひじ") != std::string::npos) {
						this->RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("右手首")) {
						this->RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("右手先") || p == std::string("右手首先")) {
						this->RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひじ") != std::string::npos) {
						this->LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("左手首")) {
						this->LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("左手先") || p == std::string("左手首先")) {
						this->LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
				}
			}

			void copy_frame(MV1& mine, fs& frame_tgt_, MV1* tgt) {
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
				for (int i = 0; i < tgt->get_anime().size(); i++) {
					tgt->get_anime(i).per = mine.get_anime(i).per;
					tgt->get_anime(i).time = mine.get_anime(i).time;
				}
			}
		};
		class opes {
		public:
			//
			bool sort_ = false;
			bool sort_f = false;
			bool sort_ing = false;
			switchs sortmag;
			//
			bool get_ = false;
			switchs getmag;
			//
			bool delete_ = false;
			switchs delete_item;
			//
			bool aim = false;
			switchs ads;
			//
			switchs squat;
			//セレクター
			bool select = false;
			switchs selkey;
			//
			bool wkey = false;
			bool skey = false;
			bool akey = false;
			bool dkey = false;
			bool qkey = false;
			bool ekey = false;
			//
			bool running = false;										//走るか否か
			bool jamp = false;
			bool shoot = false;
			bool reload = false;
			bool down_mag = false;

			void Reset_() {
				this->wkey = false;
				this->skey = false;
				this->akey = false;
				this->dkey = false;
				this->shoot = false;
				this->running = false;
				this->qkey = false;
				this->ekey = false;
				this->aim = false;
				this->reload = false;
				this->get_ = false;
				this->sort_ = false;
				this->delete_ = false;
				this->select = false;
				this->down_mag = true;
				this->jamp = false;
				this->ads.ready(false);
				this->squat.ready(false);

			}
			void Set_(const bool&isVR) {
				if (!isVR) {
					this->wkey = (CheckHitKey(KEY_INPUT_W) != 0);
					this->skey = (CheckHitKey(KEY_INPUT_S) != 0);
					this->akey = (CheckHitKey(KEY_INPUT_A) != 0);
					this->dkey = (CheckHitKey(KEY_INPUT_D) != 0);
					this->running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
					this->squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
					this->qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
					this->ekey = (CheckHitKey(KEY_INPUT_E) != 0);
					this->aim = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0);
					this->reload = (CheckHitKey(KEY_INPUT_R) != 0);
					this->get_ = (CheckHitKey(KEY_INPUT_F) != 0);
					this->sort_ = (CheckHitKey(KEY_INPUT_Z) != 0);
					this->delete_ = (CheckHitKey(KEY_INPUT_G) != 0);
					this->select = ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
					this->down_mag = true;
					this->shoot = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
					this->jamp = (CheckHitKey(KEY_INPUT_SPACE) != 0);
				}
				else {

				}
			}
			void rule_() {
				if ((this->skey) || (this->ads.on()) || (!this->wkey) || (!this->wkey && !this->skey && !this->akey && !this->dkey)) {
					this->running = false;
				}
				if (this->running) {
					this->squat.first = false;
					this->skey = false;
					this->qkey = false;
					this->ekey = false;
				}
			}
			void update() {
				//ADS
				this->ads.first = this->aim;
				//セレクター(中ボタン)
				this->selkey.get_in(this->select);
				//銃取得
				this->getmag.get_in(this->get_);
				//
				this->sortmag.get_in(this->sort_);
				//
				this->delete_item.get_in(this->delete_);
			}
		};
	public:
		class g_parts {
		public:
			size_t type = 0;
			MV1 obj, col;
			bool attach = false;
			GraphHandle reticle;
			bool state1 = false;
			performance per;

			gunparts* thisparts = nullptr;
			std::string name;
			std::array<std::array<frames, 2>, EnumAttachPoint::NUM_> rail_f;
			frames rad_parts;
			frames sight_parts;
			frames source_parts;
			std::array<frames, 2> LEFT_parts;
			std::array<frames, 2> magazine_f;
			g_parts* attach_parts = nullptr;
			frames attach_frame;
			frames mazzule_frame;
			VECTOR_ref vec;

			g_parts() {
			}

			void Reset_frame() {
				for (auto& r : this->rail_f) {
					r[0].first = -1;
					r[1].first = -1;
				}
				rad_parts.first = -1;
				attach_frame.first = -1;
			}
			void get_frame(gunparts& parts, const size_t& id) {
				if (this->attach) {
					this->thisparts = &parts;
					this->type = id;
					{
						this->name = this->thisparts->mod.name;
						this->Reset_frame();
						SetUseASyncLoadFlag(FALSE);
						if (id == EnumGunParts::PARTS_BASE) {
							this->thisparts->mod.model.DuplicateonAnime(&this->obj);
						}
						else {
							this->obj = this->thisparts->mod.model.Duplicate();
						}

						/*
						if (id == EnumGunParts::PARTS_SIGHT) {
							this->col = this->thisparts->mod.model.Duplicate();
						}
						*/

						SetUseASyncLoadFlag(FALSE);
						this->per = this->thisparts->per;
						for (int i = 0; i < int(this->obj.frame_num()); i++) {
							std::string p = this->obj.frame_name(i);
							if (p == std::string("rail")) {
								this->rail_f[EnumAttachPoint::UPER][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UPER][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("underrail")) {
								this->rail_f[EnumAttachPoint::UNDER][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UNDER][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("siderail")) {
								this->rail_f[EnumAttachPoint::LEFTSIDE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::LEFTSIDE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("rightrail")) {
								this->rail_f[EnumAttachPoint::RIGHTSIDE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::RIGHTSIDE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("sidemount")) {
								this->rail_f[EnumAttachPoint::SIDEMOUNT][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::SIDEMOUNT][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("sidemount_base")) {
								this->rail_f[EnumAttachPoint::SIDEMOUNT_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::SIDEMOUNT_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("stock")) {
								this->rail_f[EnumAttachPoint::STOCK_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::STOCK_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("dustcover")) {
								this->rail_f[EnumAttachPoint::DUSTCOVER_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::DUSTCOVER_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("uper_handguard")) {
								this->rail_f[EnumAttachPoint::UPER_HANDGUARD][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UPER_HANDGUARD][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("under_handguard")) {
								this->rail_f[EnumAttachPoint::UNDER_HANDGUARD][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UNDER_HANDGUARD][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("mazzule")) {
								this->rail_f[EnumAttachPoint::MAZZULE_][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::MAZZULE_][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("grip")) {
								this->rail_f[EnumAttachPoint::GRIP_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::GRIP_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("rad_parts")) {
								this->rad_parts = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("sight")) {
								this->sight_parts = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("LEFT")) {
								this->LEFT_parts[0] = { int(i),this->obj.frame(i) };
								this->LEFT_parts[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("source")) {
								this->source_parts = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("mag_fall")) {
								this->magazine_f[0] = { int(i),this->obj.frame(i) };
								this->magazine_f[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("mazzule")) {
								this->mazzule_frame = { int(i),this->obj.frame(i) };
							}
						}
					}
					switch (this->type) {
					case EnumGunParts::PARTS_MAZZULE:
						state1 = (this->thisparts->mazzule_id == 1);
						break;
					case EnumGunParts::PARTS_SIGHT:
						SetUseASyncLoadFlag(FALSE);
						this->reticle = this->thisparts->reticle.Duplicate();
						SetUseASyncLoadFlag(FALSE);
						this->obj.SetupCollInfo(1, 1, 1, 0, 1);

						break;
					default:
						break;
					}
				}
			}
			template<class Y, class D>
			void Draw_reticle(std::unique_ptr<Y, D>& MAPPTs) {
				if (this->attach) {
					if (this->type == EnumGunParts::PARTS_SIGHT) {
						auto p = this->obj.frame(3);
						auto p_o = p;
						auto camp = VECTOR_ref(GetCameraPosition());

						MAPPTs->map_col_nearest(camp, &p);
						//p = camp + (p - camp)*0.99f;
						if (this->obj.CollCheck_Line(camp, p, 0, 1).HitFlag == TRUE) {
							SetUseZBuffer3D(FALSE);
							DrawBillboard3D(p.get(), 0.5f, 0.5f,
								4.f*((p - camp).size() / (p_o - camp).size()),
								0.f, this->reticle.get(), TRUE);
							SetUseZBuffer3D(TRUE);
						}
					}
				}
			}
			void Set_reticle() {
				if (this->attach) {
					this->obj.RefreshCollInfo(0, 1);
				}
			}
			void attach_(performance& per_tgt, gunparts& partsptr, const size_t& id, g_parts* parents = nullptr, size_t side = 0) {
				this->attach = false;
				if (parents != nullptr) {
					if (parents->thisparts != nullptr) {
						for (auto& p : parents->thisparts->can_attach) {
							if (p == partsptr.mod.name) {
								this->attach = (parents->rail_f[side][0].first > 0);
								if (this->attach) {
									this->get_frame(partsptr, id);
									per_tgt += this->per;
								}
								if (this->attach) {
									this->attach_parts = parents;
									this->attach_frame = parents->rail_f[side][0];
									this->vec = parents->rail_f[side][1].second.Norm();
								}
								break;
							}
						}
					}
				}
			}
			void detach_(performance& per_tgt) {
				if (this->attach) {
					this->attach = false;
					this->Reset_frame();
					this->thisparts = nullptr;
					this->attach_parts = nullptr;

					this->name = "";
					this->type = EnumGunParts::PARTS_NONE;
					MV1TerminateCollInfo(this->obj.get(), 0, 1);
					this->obj.Dispose();
					this->reticle.Dispose();
					this->state1 = false;
					per_tgt -= this->per;
				}
			}

			void Setpos_parts(const MATRIX_ref& mat_t, const size_t& sel2) {
				if (this->attach) {
					auto fpos_t = MATRIX_ref(mat_t)*MATRIX_ref::Mtrans(this->attach_parts->obj.frame(this->attach_frame.first));
					if (sel2 == 1)
					{
						fpos_t = fpos_t * MATRIX_ref::Mtrans(mat_t.pos()*-1.f);
					}
					this->obj.SetMatrix(MATRIX_ref::RotVec2(this->rad_parts.second.Norm(), this->vec)*fpos_t);//方向指定
				}
			}
		};
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<Ammos, 64> bullet;
		std::array<ammo_obj, 64> cart;
		size_t use_bullet = 0;
		/*武器データ*/
		std::vector<gun_state> gun_stat;		//所持弾数などのデータ
		/*音声*/
		Audios audio;
		/*モデル*/
		g_parts base;
		g_parts magazine;
		g_parts mazzule;
		g_parts grip;
		g_parts uperhandguard;
		g_parts underhandguard;
		std::vector<g_parts> mount_;
		g_parts dustcover;
		g_parts stock;
		g_parts foregrip;
		g_parts laser;
		g_parts light;
		std::vector<g_parts> sight_;
		//
		performance per;
		MV1 body, col;
		/*anime*/
		//脚
		MV1::ani* gunanime_first = nullptr;
		MV1::ani* gunanime_shot = nullptr;
		MV1::ani* gunanime_shot_last = nullptr;
		MV1::ani* gunanime_trigger = nullptr;
		MV1::ani* gunanime_magcatch = nullptr;
		std::vector<MV1::ani*>gunanime_sel;
		/*anime*/
		//脚
		MV1::ani* anime_walk = nullptr;
		MV1::ani* anime_swalk = nullptr;
		MV1::ani* anime_run = nullptr;
		//脚ポーズ
		MV1::ani* anime_sit = nullptr;
		MV1::ani* anime_wake = nullptr;
		//死
		MV1::ani* anime_die_1 = nullptr;
		//リロード
		MV1::ani* anime_reload = nullptr;
		//腕
		MV1::ani* anime_arm_run = nullptr;
		MV1::ani* anime_arm_check = nullptr;
		//手
		MV1::ani* anime_hand_nomal = nullptr;			//基本手
		MV1::ani* anime_hand_trigger = nullptr;			//引き金
		MV1::ani* anime_hand_trigger_pull = nullptr;	//引き金引く
		/*プレイヤー操作*/
		float reload_cnt = 0.f;//リロード開始までのカウント
		switchs trigger;//トリガー
		bool gunf = false;//射撃フラグ
		bool LEFT_hand = false;//左手を添えているか
		bool reloadf = false;
		//プレイヤー座標系
		float speed = 0.f;			//
		VECTOR_ref pos;				//位置
		MATRIX_ref mat;				//位置
		MATRIX_ref mat_body;		//位置
		VECTOR_ref spawn_pos;		//spawn
		MATRIX_ref spawn_mat;		//spawn
		VECTOR_ref add_vec;			//移動ベクトル
		VECTOR_ref add_vec_buf;		//移動ベクトルバッファ
		VECTOR_ref add_vec_real;	//実際の移動ベクトル
		float xrad_p = 0.f;			//マウスエイム用変数確保
		opes key_;					//
		bool sort_f = false;		//
		bool sort_ing = false;		//走るか否か
		VECTOR_ref gunpos;			//マウスエイム用銃座標
		//AI用
		float ai_time_a = 0.f;
		float ai_time_d = 0.f;
		float ai_time_q = 0.f;
		float ai_time_e = 0.f;
		float ai_time_shoot = 0.f;
		bool ai_reload = false;
		int ai_phase = 0;
		std::array<int, 6> wayp_pre;
		//マガジン座標系
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//銃座標
		VECTOR_ref pos_gun;
		MATRIX_ref mat_gun;
		VECTOR_ref addpos_gun;
		VECTOR_ref recoil_vec, recoil_vec_res;
		//頭部座標系
		VECTOR_ref pos_HMD, pos_HMD_old, rec_HMD;
		float add_ypos = 0.f;		//垂直加速度
		float body_xrad = 0.f;		//胴体角度
		float body_yrad = 0.f;		//胴体角度
		float body_zrad = 0.f;		//胴体角度
		float body_ztrun = 0.f;		//胴体角度
		//フレーム
		fs frame_s;
		float head_hight = 0.f;
		float head_hight2 = 0.f;
		//右手座標系
		VECTOR_ref pos_RIGHTHAND;
		MATRIX_ref mat_RIGHTHAND;
		//左手座標系
		VECTOR_ref pos_LEFTHAND;
		MATRIX_ref mat_LEFTHAND;
		//右手座標系
		MATRIX_ref mat_RIGHTREG_rep;
		VECTOR_ref pos_RIGHTREG;
		MATRIX_ref mat_RIGHTREG;
		//左手座標系
		MATRIX_ref mat_LEFTREG_rep;
		VECTOR_ref pos_LEFTREG;
		MATRIX_ref mat_LEFTREG;
		//
		VECTOR_ref pos_WAIST_rep;
		MATRIX_ref mat_WAIST_rep;
		VECTOR_ref pos_WAIST;
		MATRIX_ref mat_WAIST;
		//
		fs colframe_;
		//
		size_t canget_id = 0;
		//
		bool canget_gunitem = false;
		std::string canget_gun;
		//
		bool canget_magitem = false;
		std::string canget_mag;
		//
		bool canget_meditem = false;
		std::string canget_med;
		//
		bool Start_b = true;
		bool Start_c = true;
		float HP_r = 100;
		int HP = 100;
		int HP_full = 100;
		bool kill_f = false;
		size_t kill_id = 0;
		float kill_time = 0.f;
		int kill_streak = 0;
		bool hit_f = false;
		float hit_time = 0.f;
		int score = 0;
		int kill_count = 0;
		size_t death_id = 0;
		int death_count = 0;
		float death_timer = 0.f;
		//
		bool oldv_1_1 = false;
		bool oldv_1_2 = false;
		bool oldv_2_1 = false;
		bool oldv_2_2 = false;
		bool oldv_3_1 = false;
		bool oldv_3_2 = false;
		//
		float ratio_r = 0.f;
		//ライト
		int LightHandle = -1;
		//
		void Set(std::vector<gunparts>& gun_data, const size_t& itr, MV1& body_, MV1& col_) {
			//変数
			{
				this->gunf = false;
				this->reloadf = false;
				this->LEFT_hand = false;
				this->recoil_vec = VGet(0, 0, 1.f);
				this->recoil_vec_res = this->recoil_vec;
				this->HP = this->HP_full;
				this->kill_f = false;
				this->kill_id = 0;
				this->kill_streak = 0;
				this->kill_time = 0.f;
				this->score = 0;
				this->kill_count = 0;
				this->death_id = 0;
				this->death_count = 0;
				this->per.recoil = 0.f;
				this->per.weight = 0.f;
				this->LightHandle = -1;
			}
			//base
			add_parts(&gun_data[itr], EnumGunParts::PARTS_BASE);
			//身体
			{
				body_.DuplicateonAnime(&this->body);
				//
				this->anime_walk = &this->body.get_anime(1);
				this->anime_swalk = &this->body.get_anime(8);
				this->anime_run = &this->body.get_anime(2);
				//
				this->anime_sit = &this->body.get_anime(7);
				this->anime_wake = &this->body.get_anime(11);
				//
				this->anime_die_1 = &this->body.get_anime(4);
				//
				this->anime_reload = &this->body.get_anime(3);
				//
				this->anime_arm_run = &this->body.get_anime(6);
				this->anime_arm_check = &this->body.get_anime(10);
				//
				this->anime_hand_nomal = &this->body.get_anime(0);
				this->anime_hand_trigger = &this->body.get_anime(9);
				this->anime_hand_trigger_pull = &this->body.get_anime(5);
				//
				this->frame_s.get_frame(this->body, &this->head_hight);
			}
			//身体コリジョン
			{
				col_.DuplicateonAnime(&this->col);
				this->colframe_.get_frame(this->col, &this->head_hight2);
				for (int i = 0; i < this->col.mesh_num(); i++) {
					this->col.SetupCollInfo(8, 8, 8, -1, i);
				}
			}
			//gun
			{
				gunanime_sel.resize(this->base.thisparts->select.size());
				for (auto& a : this->base.obj.get_anime()) {
					a.reset();
				}
				this->gunanime_shot = &this->base.obj.get_anime(0);
				this->gunanime_shot_last = &this->base.obj.get_anime(1);
				this->gunanime_trigger = &this->base.obj.get_anime(2);
				this->gunanime_magcatch = &this->base.obj.get_anime(5);
				this->gunanime_sel[0] = &this->base.obj.get_anime(4);
				this->gunanime_sel[1] = &this->base.obj.get_anime(7);
				this->gunanime_first = &this->base.obj.get_anime(6);
			}
			//
			{
				this->gun_stat.resize(gun_data.size());
				std::for_each(this->gun_stat.begin(), this->gun_stat.end(), [](gun_state& i) {i.init(); });
			}

			//薬莢
			for (auto& a : this->cart) {
				a.Ready();
			}
			//bullet
			for (auto& a : this->bullet) {
				a.Ready();
			}
			//audio
			this->audio.Duplicate(this->base.thisparts->audio);
		}
		void add_parts(gunparts* partsptr, const size_t& id, g_parts* parents = nullptr, size_t side = 0) {
			if (partsptr != nullptr) {
				switch (id) {
				case EnumGunParts::PARTS_BASE:
				{
					this->base.attach = true;
					if (this->base.attach) {
						this->base.get_frame(*partsptr, id);
						this->per += this->base.per;
					}
					break;
				}
				case EnumGunParts::PARTS_MAGAZINE:
				{
					this->magazine.attach = true;
					if (this->magazine.attach) {
						this->magazine.get_frame(*partsptr, id);
						this->per += this->magazine.per;
					}
					break;
				}
				case EnumGunParts::PARTS_MAZZULE:
				{
					this->mazzule.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_GRIP:
				{
					this->grip.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_UPER_HGUARD:
				{
					this->uperhandguard.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_UNDER_HGUARD:
				{
					this->underhandguard.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_DUSTCOVER:
				{
					this->dustcover.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_STOCK:
				{
					this->stock.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_LASER:
				{
					this->laser.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_LIGHT:
				{
					this->light.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_FOREGRIP:
				{
					this->foregrip.attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_MOUNT:
				{
					//マウントにマウントを付ける特殊処理
					if (parents != nullptr) {
						int onid = -1;
						for (auto& p : this->mount_) {
							if (&p == parents) {
								onid = int(&p - &this->mount_[0]);
							}
						}
						this->mount_.resize(this->mount_.size() + 1);
						if (onid >= 0) {
							parents = &this->mount_[onid];
						}
					}
					//
					this->mount_.back().attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				case EnumGunParts::PARTS_SIGHT:
				{
					if (parents != nullptr) {
						this->sight_.resize(this->sight_.size() + 1);
					}
					this->sight_.back().attach_(this->per, *partsptr, id, parents, side);
					break;
				}
				default:
					break;
				}
			}
		}
		void check_empty_mount_slot() {
			while (true) {
				int t = -1;
				for (auto& m : this->mount_) {
					if (!m.attach) {
						t = int(&m - &this->mount_[0]);
						break;
					}
				}
				if (t != -1) {
					this->mount_.erase(this->mount_.begin() + t);
				}
				else {
					break;
				}
			}
		}
		void check_empty_sight_slot() {
			while (true) {
				int t = -1;
				for (auto& s : this->sight_) {
					if (!s.attach) {
						t = int(&s - &this->sight_[0]);
						break;
					}
				}
				if (t != -1) {
					this->sight_.erase(this->sight_.begin() + t);
				}
				else {
					break;
				}
			}
		}
		void delete_child(g_parts* parent) {
			if (this->mazzule.attach_parts == parent) {
				this->mazzule.detach_(this->per);
			}
			if (this->grip.attach_parts == parent) {
				this->grip.detach_(this->per);
			}
			if (this->uperhandguard.attach_parts == parent) {
				this->uperhandguard.detach_(this->per);
			}
			if (this->underhandguard.attach_parts == parent) {
				this->underhandguard.detach_(this->per);
			}
			//
			for (auto& m : this->mount_) {
				if (m.attach_parts == parent) {
					m.detach_(this->per);
				}
			}
			check_empty_mount_slot();
			//
			if (this->dustcover.attach_parts == parent) {
				this->dustcover.detach_(this->per);
			}
			if (this->stock.attach_parts == parent) {
				this->stock.detach_(this->per);
			}
			if (this->foregrip.attach_parts == parent) {
				this->foregrip.detach_(this->per);
			}
			if (this->laser.attach_parts == parent) {
				this->laser.detach_(this->per);
			}
			if (this->light.attach_parts == parent) {
				this->light.detach_(this->per);
			}
			//
			for (auto& s : this->sight_) {
				if (s.attach_parts == parent) {
					s.detach_(this->per);
				}
			}
			check_empty_sight_slot();
		}
		void delete_parts(const size_t& id, const int& sight_mount = 0) {
			switch (id) {
			case EnumGunParts::PARTS_MAGAZINE:
			{
				this->delete_child(&this->magazine);
				this->magazine.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_BASE:
			{
				this->delete_child(&this->base);
				this->base.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_MAZZULE:
			{
				this->delete_child(&this->mazzule);
				this->mazzule.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_GRIP:
			{
				this->delete_child(&this->grip);
				this->grip.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_UPER_HGUARD:
			{
				this->delete_child(&this->uperhandguard);
				this->uperhandguard.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_UNDER_HGUARD:
			{
				this->delete_child(&this->underhandguard);
				this->underhandguard.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_DUSTCOVER:
			{
				this->delete_child(&this->dustcover);
				this->dustcover.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_STOCK:
			{
				this->delete_child(&this->stock);
				this->stock.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_LASER:
			{
				this->delete_child(&this->laser);
				this->laser.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_LIGHT:
			{
				this->delete_child(&this->light);
				this->light.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_FOREGRIP:
			{
				this->delete_child(&this->foregrip);
				this->foregrip.detach_(this->per);
				break;
			}
			case EnumGunParts::PARTS_SIGHT:
			{
				if (this->sight_.size() > 0) {
					this->delete_child(&this->sight_[std::min(int(this->sight_.size()) - 1, sight_mount)]);
					if (this->sight_.size() > 0) {
						this->sight_[std::min(int(this->sight_.size()) - 1, sight_mount)].detach_(this->per);
					}
					check_empty_sight_slot();
				}
				break;
			}
			case EnumGunParts::PARTS_MOUNT:
			{
				if (this->mount_.size() > 0) {
					this->delete_child(&this->mount_[std::min(int(this->mount_.size()) - 1, sight_mount)]);
					if (this->mount_.size() > 0) {
						this->mount_[std::min(int(this->mount_.size()) - 1, sight_mount)].detach_(this->per);
					}
					check_empty_mount_slot();
				}
				break;
			}
			default:
				break;
			}
		}
		void spawn(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) {
			this->ai_time_shoot = 0.f;
			this->ai_time_a = 0.f;
			this->ai_time_d = 0.f;
			this->ai_time_e = 0.f;

			this->xrad_p = 0;

			this->spawn_pos = pos_;
			this->spawn_mat = mat_H;

			this->pos = this->spawn_pos;
			this->mat = this->spawn_mat;

			this->key_.Reset_();

			this->add_ypos = 0.f;
			this->add_vec_buf.clear();
			this->Start_b = true;

			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->death_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;

			this->gun_stat[this->base.thisparts->id].init();
			this->gun_stat[this->base.thisparts->id].mag_insert(this->magazine.thisparts);			//マガジン+1(装填あり)
			this->gun_stat[this->base.thisparts->id].mag_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat[this->base.thisparts->id].mag_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat[this->base.thisparts->id].mag_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat[this->base.thisparts->id].mag_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat[this->base.thisparts->id].mag_plus(this->magazine.thisparts);			//マガジン+1(装填無し)

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度
			std::for_each(this->wayp_pre.begin(), this->wayp_pre.end(), [](int& i) {i = 0; });
			this->gunf = false;
			this->gunanime_shot->reset();
			this->gunanime_shot_last->reset();
			this->gunanime_trigger->reset();
			this->gunanime_sel[0]->reset();
			this->gunanime_magcatch->reset();
			this->gunanime_first->reset();
			this->gunanime_sel[1]->reset();
		}
		template<class Y, class D>
		void Start(std::unique_ptr<Y, D>& MAPPTs) {
			this->oldv_1_1 = true;
			this->oldv_1_2 = true;
			this->oldv_2_1 = true;
			this->oldv_2_2 = true;
			this->oldv_3_1 = true;
			this->oldv_3_2 = true;
			this->ratio_r = 0.f;
			this->Start_b = false;
			this->Start_c = true;
			this->Reset_waypoint(MAPPTs);
			this->gunanime_first->per = 1.f;
		}
		template<class Y, class D>
		void Reset_waypoint(std::unique_ptr<Y, D>& MAPPTs) {
			int now = -1;
			auto poss = this->pos + this->pos_HMD - this->rec_HMD;
			auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
			for (auto& w : MAPPTs->get_waypoint()) {
				auto id = &w - &MAPPTs->get_waypoint()[0];
				bool tt = true;
				for (auto& ww : this->wayp_pre) {
					if (id == ww) {
						tt = false;
					}
				}
				if (tt) {
					if (tmp.size() >= (w - poss).size()) {
						auto p = MAPPTs->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
						if (!p.HitFlag) {
							tmp = (w - poss);
							now = int(id);
						}
					}
				}
			}
			if (now != -1) {
				for (auto& w : this->wayp_pre) {
					w = now;
				}
			}
			this->ai_phase = 0;
		}
		void magrelease_tp(std::vector<Items>& item, const size_t& dnm) {
			const auto fps_ = GetFPS();
			bool tt = false;
			for (auto& g : item) {
				if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
					tt = true;
					g.Set_item(this->magazine.thisparts, this->pos_mag, this->mat_mag);
					g.add = (this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
					g.magazine.cap = dnm;
					g.del_timer = 0.f;
					break;
				}
			}
			if (!tt) {
				item.resize(item.size() + 1);
				auto& g = item.back();
				g.id = item.size() - 1;
				g.Set_item(this->magazine.thisparts, this->pos_mag, this->mat_mag);
				g.add = (this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
				/*
				g.add = VECTOR_ref(VGet(
					float(-10 + GetRand(10 * 2)) / 100.f,
					-1.f,
					float(-10 + GetRand(10 * 2)) / 100.f
				))*1.f / fps_;//排莢ベクトル
				*/
				g.magazine.cap = dnm;
				g.del_timer = 0.f;
			}
		}
		void magrelease_t(std::vector<Items>& item, const size_t& test_) {
			const auto fps_ = GetFPS();
			if (this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
				//音
				this->audio.mag_down.play_3D(this->pos_gun, 15.f);
				//弾数
				auto dnm = (this->gun_stat[this->base.thisparts->id].ammo_cnt >= 1) ? this->gun_stat[this->base.thisparts->id].ammo_cnt - 1 : 0;
				if (test_ == 0) {
					while (true) {
						this->gun_stat[this->base.thisparts->id].mag_release();
						magrelease_tp(item, dnm);
						//
						if (this->gun_stat[this->base.thisparts->id].mag_in.size() == 0) {
							break;
						}
						dnm = this->gun_stat[this->base.thisparts->id].mag_in.front();
					}
					return;
				}
				if (test_ == 1) {
					this->gun_stat[this->base.thisparts->id].mag_release();
					magrelease_tp(item, dnm);
					this->reload_cnt = 0.f;
					return;
				}
			}
			else {
				if (test_ == 2) {
					this->gun_stat[this->base.thisparts->id].mag_release_end();
					magrelease_tp(item, 0);
					this->reload_cnt = 0.f;
					return;
				}
			}
		}
		//
		template<class Y2, class D2>
		void Set_alive(std::vector<Items>& item, std::unique_ptr<Y2, D2>& MAPPTs) {
			const auto fps_ = GetFPS();
			easing_set(&this->HP_r, float(this->HP), 0.95f);
			if (this->HP == 0) {
				this->pos_HMD.clear();
				easing_set(&this->anime_hand_nomal->per, 0.f, 0.9f);
				easing_set(&this->anime_walk->per, 0.f, 0.9f);
				easing_set(&this->anime_run->per, 0.f, 0.9f);
				easing_set(&this->anime_reload->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger_pull->per, 0.f, 0.9f);
				easing_set(&this->anime_arm_run->per, 0.f, 0.9f);
				easing_set(&this->anime_arm_check->per, 0.f, 0.9f);
				easing_set(&this->anime_sit->per, 0.f, 0.9f);
				easing_set(&this->anime_wake->per, 0.f, 0.9f);
				easing_set(&this->anime_swalk->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger->per, 0.f, 0.9f);
				//
				easing_set(&this->anime_die_1->per, 1.f, 0.9f);
				this->anime_die_1->update(false, 1.f);
				//
				if (this->death_timer == 0.f) {
					this->body.frame_reset(this->frame_s.bodyg_f.first);
					this->body.frame_reset(this->frame_s.bodyb_f.first);
					this->body.frame_reset(this->frame_s.body_f.first);
					this->body.frame_reset(this->frame_s.head_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm1_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm1_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm2_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm2_f.first);
					this->body.frame_reset(this->frame_s.RIGHThand_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm1_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm1_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm2_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm2_f.first);
					this->body.frame_reset(this->frame_s.LEFThand_f.first);
					magrelease_t(item, 0);//マガジン排出
				}
				this->death_timer += 1.f / fps_;
				if (this->death_timer >= 3.f) {
					this->death_timer = 0.f;
					this->anime_die_1->reset();
					this->spawn(this->spawn_pos, this->spawn_mat);
					Reset_waypoint(MAPPTs);
				}

			}
			if (this->kill_f) {
				this->kill_time -= 1.f / fps_;
				if (this->kill_time <= 0.f) {
					this->kill_time = 0.f;
					this->kill_streak = 0;
					this->kill_f = false;
				}
			}
		}
		//
		void Set_gun_haveto() {
		}
		void Set_gun1(const MATRIX_ref& mat_T) {
			this->base.obj.SetMatrix(MATRIX_ref(mat_T) * MATRIX_ref::Mtrans(this->pos_gun));
			{
				this->underhandguard.Setpos_parts(mat_T, 0);
				this->uperhandguard.Setpos_parts(mat_T, 0);
				this->grip.Setpos_parts(mat_T, 0);
				this->mazzule.Setpos_parts(mat_T, 0);
				this->dustcover.Setpos_parts(mat_T, 0);
				this->stock.Setpos_parts(mat_T, 0);
				this->foregrip.Setpos_parts(mat_T, 0);
				this->light.Setpos_parts(mat_T, 0);
				this->laser.Setpos_parts(mat_T, 0);

				for (auto&m : this->mount_) {
					m.Setpos_parts(mat_T, 0);
				}
				for (auto&s : this->sight_) {
					s.Setpos_parts(mat_T, 0);
				}
			}
		}
		//
		void Set_gun() {
			Set_gun1(this->mat_gun);
		}
		void Set_gun3() {
			auto mat_T = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->recoil_vec)*this->mat;//リコイル
			this->pos_gun = this->pos + this->pos_HMD + MATRIX_ref::Vtrans(this->gunpos, mat_T);
			Set_gun1(mat_T);
		}
		void Set_gun2() {
			auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* this->body.GetFrameLocalWorldMatrix(this->frame_s.RIGHThand2_f.first);
			this->pos_gun = this->body.frame(this->frame_s.RIGHThand_f.first);
			//this->Set_gun();
			this->base.obj.SetMatrix(mat_T);
			this->base.obj.SetMatrix(mat_T*MATRIX_ref::Mtrans(this->pos_gun - this->base.obj.frame(this->base.thisparts->frame_s.right_f.first)));
			{
				this->underhandguard.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->uperhandguard.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->grip.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->mazzule.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->dustcover.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->stock.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->foregrip.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->light.Setpos_parts(this->base.obj.GetMatrix(), 1);
				this->laser.Setpos_parts(this->base.obj.GetMatrix(), 1);

				for (auto&m : this->mount_) {
					m.Setpos_parts(this->base.obj.GetMatrix(), 1);
				}
				for (auto&s : this->sight_) {
					s.Setpos_parts(this->base.obj.GetMatrix(), 1);
				}
			}
			//
			this->mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (this->body.GetFrameLocalWorldMatrix(this->frame_s.LEFThand2_f.first)*MATRIX_ref::Mtrans(this->body.frame(this->frame_s.LEFThand2_f.first)).Inverse());
			this->pos_LEFTHAND = this->body.frame(this->frame_s.LEFThand_f.first) + this->mat_LEFTHAND.yvec()*0.1f;
		}
		//
		template<class Y2, class D2>
		void wall_col(std::unique_ptr<Y2, D2>& MAPPTs, Chara& mine, const bool& use_vr) {
			//VR用
			{
				VECTOR_ref pos_t2 = this->pos + (VECTOR_ref(VGet(this->pos_HMD_old.x(), 0.f, this->pos_HMD_old.z())) - this->rec_HMD);
				VECTOR_ref pos_t = this->pos + (VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				this->pos = pos_t - (VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
			}
			//共通
			{
				VECTOR_ref pos_t2 = (this->pos + (VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
				VECTOR_ref pos_t = pos_t2 + this->add_vec;
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				//落下
				{
					auto pp = MAPPTs->map_col_line(pos_t + VGet(0, 1.8f, 0), pos_t);
					if (this->add_ypos <= 0.f && pp.HitFlag) {
						pos_t = pp.HitPosition;
						this->add_ypos = 0.f;
					}
					else {
						pos_t.yadd(this->add_ypos);
						this->add_ypos += M_GR / std::powf(GetFPS(), 2.f);
						//復帰
						if (pos_t.y() <= -5.f) {
							pos_t = this->spawn_pos;
							if (!use_vr && (this == &mine)) {
								this->xrad_p = 0;
							}
							this->spawn(pos_t, this->spawn_mat);
							this->Reset_waypoint(MAPPTs);
						}
					}
				}
				//反映
				this->pos = pos_t - (VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
				this->add_vec_real = pos_t - pos_t2;
			}
		}
		//
		void move_righthand(const bool& isusewaist, const MATRIX_ref& m_inv, bool ptt) {
			//基準
			this->pos_RIGHTHAND = this->base.obj.frame(this->base.thisparts->frame_s.right_f.first);
			if (!ptt) {
				this->mat_RIGHTHAND = this->mat_gun;
			}
			VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((this->pos_RIGHTHAND - this->body.frame(this->frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
			VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(-1.f*sin(deg2rad(30)), -1.f*cos(deg2rad(30)), vec_a1.y() / vec_a1.z())).Norm();//x=0とする
			float cos_t = getcos_tri((this->body.frame(this->frame_s.RIGHThand_f.first) - this->body.frame(this->frame_s.RIGHTarm2_f.first)).size(), (this->body.frame(this->frame_s.RIGHTarm2_f.first) - this->body.frame(this->frame_s.RIGHTarm1_f.first)).size(), (this->body.frame(this->frame_s.RIGHTarm1_f.first) - this->pos_RIGHTHAND).size());
			VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			if ((ptt && !isusewaist) || !ptt) {
				this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, (this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHTarm2_f.first) - this->body.frame(this->frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
			if ((ptt && !isusewaist) || !ptt) {
				this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*(this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			//下腕
			this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHThand_f.first) - this->body.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->pos_RIGHTHAND - this->body.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
			this->body.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
			//手
			this->body.SetFrameLocalMatrix(this->frame_s.RIGHThand_f.first,
				MATRIX_ref::RotZ(deg2rad(30))*
				MATRIX_ref::RotX(deg2rad(80))*
				this->mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHThand_f.second));
		}
		void move_lefthand(const bool& isusewaist, const MATRIX_ref& m_inv, bool ptt) {
			//基準
			VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((this->pos_LEFTHAND - this->body.frame(this->frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
			VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
			float cos_t = getcos_tri((this->body.frame(this->frame_s.LEFThand_f.first) - this->body.frame(this->frame_s.LEFTarm2_f.first)).size(), (this->body.frame(this->frame_s.LEFTarm2_f.first) - this->body.frame(this->frame_s.LEFTarm1_f.first)).size(), (this->body.frame(this->frame_s.LEFTarm1_f.first) - this->pos_LEFTHAND).size());
			VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			if ((ptt && !isusewaist) || !ptt) {
				this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, (this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFTarm2_f.first) - this->body.frame(this->frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
			if ((ptt && !isusewaist) || !ptt) {
				this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*(this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			//下腕
			this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFThand_f.first) - this->body.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->pos_LEFTHAND - this->body.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
			this->body.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
			//手
			this->body.SetFrameLocalMatrix(this->frame_s.LEFThand_f.first, MATRIX_ref::RotZ(deg2rad(-30))* MATRIX_ref::RotX(deg2rad(80))* this->mat_LEFTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFThand_f.second));
		}
		//
		template<class Y2, class D2>
		void move_vr(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			if (this->HP != 0) {
				VECTOR_ref v_ = this->mat.zvec();
				if (DrawPts->tracker_num.size() > 0) {
					v_ = this->mat_WAIST.zvec();
				}
				float x_1 = -sinf(this->body_yrad);
				float y_1 = cosf(this->body_yrad);
				float x_2 = v_.x();
				float y_2 = -v_.z();
				float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
				this->body_yrad += r_ * FRAME_RATE / GetFPS() / 10.f;
			}
			//身体,頭部,腰
			MATRIX_ref m_inv = MATRIX_ref::RotY((DrawPts->tracker_num.size() > 0) ? DX_PI_F : 0 + this->body_yrad);
			{
				if (this->HP != 0) {
					this->body.SetMatrix(m_inv);
					if (DrawPts->tracker_num.size() > 0) {
						//腰
						this->body.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, (this->mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
						//頭部
						this->body.SetFrameLocalMatrix(this->frame_s.head_f.first, (MATRIX_ref::Axis1(this->mat.xvec()*-1.f, this->mat.yvec(), this->mat.zvec()*-1.f) *m_inv.Inverse()*(this->mat_WAIST*m_inv.Inverse()).Inverse())
							*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
					}
					else {
						//頭部
						this->body.SetFrameLocalMatrix(this->frame_s.head_f.first, (MATRIX_ref::Axis1(this->mat.xvec()*-1.f, this->mat.yvec(), this->mat.zvec()*-1.f) *m_inv.Inverse())
							*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
					}
					this->body.SetMatrix(m_inv *MATRIX_ref::Mtrans((this->pos + this->pos_HMD - this->rec_HMD) - (this->body.frame(this->frame_s.RIGHTeye_f.first) + (this->body.frame(this->frame_s.LEFTeye_f.first) - this->body.frame(this->frame_s.RIGHTeye_f.first))*0.5f)));
				}
				else {
					m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
					this->body.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->pos + VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD));
				}
			}
			//足
			{
				//左
				if (DrawPts->tracker_num.size() > 1) {
					auto& ptr_ = (*DrawPts->get_device())[DrawPts->tracker_num[1]];
					DrawPts->GetDevicePositionVR(DrawPts->tracker_num[1], &this->pos_LEFTREG, &this->mat_LEFTREG);
					this->mat_LEFTREG = MATRIX_ref::Axis1(this->mat_LEFTREG.xvec()*-1.f, this->mat_LEFTREG.yvec(), this->mat_LEFTREG.zvec()*-1.f);
					if ((this->Start_c || (ptr_.turn && ptr_.now) != this->oldv_2_1) && this->oldv_2_2) {
						this->mat_LEFTREG_rep = this->mat_LEFTREG;
						if (!this->Start_c) {
							//this->oldv_2_2 = false;
						}
					}
					this->oldv_2_1 = ptr_.turn && ptr_.now;
					this->mat_LEFTREG =
						MATRIX_ref::RotY(deg2rad(90 + 60 - 10))*
						this->mat_LEFTREG_rep.Inverse()*this->mat_LEFTREG;
					this->pos_LEFTREG = this->pos_LEFTREG + (this->pos - this->rec_HMD);
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_LEFTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->body.frame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_LEFTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->body.frame(this->frame_s.LEFTreg_f.first) - this->body.frame(this->frame_s.LEFTfoot2_f.first)).size(), (this->body.frame(this->frame_s.LEFTfoot2_f.first) - this->body.frame(this->frame_s.LEFTfoot1_f.first)).size(), (this->body.frame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFTfoot2_f.first) - this->body.frame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

						//下腕
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFTreg_f.first) - this->body.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->body.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						//手
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
					}

					{
						auto pp = MAPPTs->map_col_line(this->body.frame(this->frame_s.LEFTreg2_f.first) + VGet(0, 1.8f, 0), this->body.frame(this->frame_s.LEFTreg2_f.first));
						if (pp.HitFlag) {
							this->pos_LEFTREG = VECTOR_ref(pp.HitPosition) - (this->body.frame(this->frame_s.LEFTreg2_f.first) - this->body.frame(this->frame_s.LEFTreg_f.first));
						}
					}

					{
						//基準
						VECTOR_ref tgt_pt = this->pos_LEFTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->body.frame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_LEFTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->body.frame(this->frame_s.LEFTreg_f.first) - this->body.frame(this->frame_s.LEFTfoot2_f.first)).size(), (this->body.frame(this->frame_s.LEFTfoot2_f.first) - this->body.frame(this->frame_s.LEFTfoot1_f.first)).size(), (this->body.frame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFTfoot2_f.first) - this->body.frame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

						//下腕
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.LEFTreg_f.first) - this->body.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->body.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						//手
						this->body.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
					}
				}
				//右
				if (DrawPts->tracker_num.size() > 2) {
					auto& ptr_ = (*DrawPts->get_device())[DrawPts->tracker_num[2]];
					DrawPts->GetDevicePositionVR(DrawPts->tracker_num[2], &this->pos_RIGHTREG, &this->mat_RIGHTREG);
					this->mat_RIGHTREG = MATRIX_ref::Axis1(this->mat_RIGHTREG.xvec()*-1.f, this->mat_RIGHTREG.yvec(), this->mat_RIGHTREG.zvec()*-1.f);
					if ((this->Start_c || (ptr_.turn && ptr_.now) != this->oldv_3_1) && this->oldv_3_2) {
						this->mat_RIGHTREG_rep = this->mat_RIGHTREG;
						if (!this->Start_c) {
							//this->oldv_3_2 = false;
						}
					}
					this->oldv_3_1 = ptr_.turn && ptr_.now;
					this->mat_RIGHTREG =
						MATRIX_ref::RotY(deg2rad(180 - 22 - 10))*
						this->mat_RIGHTREG_rep.Inverse()*this->mat_RIGHTREG;
					this->pos_RIGHTREG = this->pos_RIGHTREG + (this->pos - this->rec_HMD);
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_RIGHTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->body.frame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_RIGHTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする


						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->body.frame(this->frame_s.RIGHTreg_f.first) - this->body.frame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->body.frame(this->frame_s.RIGHTfoot2_f.first) - this->body.frame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->body.frame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHTfoot2_f.first) - this->body.frame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						//下腕
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHTreg_f.first) - this->body.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->body.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						//手
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
					}

					{
						auto pp = MAPPTs->map_col_line(this->body.frame(this->frame_s.RIGHTreg2_f.first) + VGet(0, 1.8f, 0), this->body.frame(this->frame_s.RIGHTreg2_f.first));
						if (pp.HitFlag) {
							this->pos_RIGHTREG = VECTOR_ref(pp.HitPosition) - (this->body.frame(this->frame_s.RIGHTreg2_f.first) - this->body.frame(this->frame_s.RIGHTreg_f.first));
						}
					}
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_RIGHTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->body.frame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_RIGHTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->body.frame(this->frame_s.RIGHTreg_f.first) - this->body.frame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->body.frame(this->frame_s.RIGHTfoot2_f.first) - this->body.frame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->body.frame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHTfoot2_f.first) - this->body.frame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						//下腕
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->body.frame(this->frame_s.RIGHTreg_f.first) - this->body.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->body.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						//手
						this->body.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
					}
				}
			}
			//手
			{}
			{
				this->anime_reload->reset();
				if (this->HP != 0) {
					//右手
					DrawPts->GetDevicePositionVR(DrawPts->get_hand1_num(), &this->pos_RIGHTHAND, &this->mat_RIGHTHAND);
					this->pos_RIGHTHAND = this->pos_RIGHTHAND + (this->pos - this->rec_HMD);
					this->mat_RIGHTHAND = MATRIX_ref::Axis1(this->mat_RIGHTHAND.xvec()*-1.f, this->mat_RIGHTHAND.yvec(), this->mat_RIGHTHAND.zvec()*-1.f);
					this->mat_RIGHTHAND = this->mat_RIGHTHAND*MATRIX_ref::RotAxis(this->mat_RIGHTHAND.xvec(), deg2rad(-60));
					this->mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->recoil_vec)*this->mat_RIGHTHAND;//リコイル
					//左手
					DrawPts->GetDevicePositionVR(DrawPts->get_hand2_num(), &this->pos_LEFTHAND, &this->mat_LEFTHAND);
					this->pos_LEFTHAND = this->pos_LEFTHAND + (this->pos - this->rec_HMD);
					this->mat_LEFTHAND = MATRIX_ref::Axis1(this->mat_LEFTHAND.xvec()*-1.f, this->mat_LEFTHAND.yvec(), this->mat_LEFTHAND.zvec()*-1.f);
					this->mat_LEFTHAND = this->mat_LEFTHAND*MATRIX_ref::RotAxis(this->mat_LEFTHAND.xvec(), deg2rad(-60));
				}
				//右手
				{
					//銃器
					if (this->HP != 0) {
						this->pos_gun = this->pos_RIGHTHAND;
						this->mat_gun = this->mat_RIGHTHAND;
					}
					this->Set_gun();
					//右人差し指
					this->anime_hand_nomal->per = 1.f;
					this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
					this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					move_righthand(DrawPts->tracker_num.size() > 0, m_inv, true);
				}
				//左手
				{
					{
						VECTOR_ref f_;
						if (this->foregrip.attach) {
							f_ = this->foregrip.obj.frame(this->foregrip.LEFT_parts[0].first);
						}
						else {
							f_ = this->base.obj.frame(this->base.thisparts->frame_s.left_f.first);
						}
						{
							float dist_ = (this->pos_LEFTHAND - f_).size();
							if (dist_ <= 0.1f && (!this->reloadf || !this->key_.down_mag)) {
								this->LEFT_hand = true;
								this->pos_LEFTHAND = f_;
							}
							else {
								this->LEFT_hand = false;
							}
						}
					}
					move_lefthand(DrawPts->tracker_num.size() > 0, m_inv, true);
				}
			}
		}

		void search_base(VECTOR_ref& pv, g_parts* ptr) {
			if (ptr->attach_parts != &this->base && ptr->attach_parts != nullptr) {
				pv = ptr->attach_parts->attach_frame.second + pv;
				search_base(pv, ptr->attach_parts);
			}
		}
		//
		template<class Y2, class D2>
		void move_other(std::unique_ptr<Y2, D2>& MAPPTs, Chara& mine) {
			if (this->HP != 0) {
				{
					VECTOR_ref v_ = this->mat_body.zvec();
					float x_1 = -sinf(this->body_yrad);
					float y_1 = cosf(this->body_yrad);
					float x_2 = v_.x();
					float y_2 = -v_.z();
					this->body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / GetFPS() / 6.f;
				}
				{
					VECTOR_ref v_ = this->mat_body.zvec();
					float x_1 = sinf(this->body_xrad);
					float y_1 = -cosf(this->body_xrad);
					float x_2 = -v_.y();
					float y_2 = -std::hypotf(v_.x(), v_.z());
					this->body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
				}
			}
			//身体
			MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
			MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*MATRIX_ref::RotZ(this->body_zrad);
			MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(this->body_zrad)*MATRIX_ref::RotX(this->body_xrad)*mg_inv;
			{
				//
				if (this->HP != 0) {
					this->body.SetMatrix(MATRIX_ref::Mtrans(this->pos - this->rec_HMD));
					this->body.SetFrameLocalMatrix(this->frame_s.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(this->frame_s.bodyg_f.second));
					this->body.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
					this->body.SetFrameLocalMatrix(this->frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(this->frame_s.body_f.second));

					if (this->reloadf || this->key_.running || this->anime_arm_check->per == 1.f) {
						mb_inv = MATRIX_ref::RotZ(this->body_zrad);
						m_inv = MATRIX_ref::RotZ(this->body_zrad)* MATRIX_ref::RotX(this->body_xrad)*mg_inv;
						this->body.frame_reset(this->frame_s.bodyg_f.first);
						this->body.frame_reset(this->frame_s.bodyb_f.first);
						this->body.frame_reset(this->frame_s.body_f.first);
						this->body.SetFrameLocalMatrix(this->frame_s.bodyg_f.first, mg_inv*this->body.GetFrameLocalMatrix(this->frame_s.bodyg_f.first));
						this->body.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, mb_inv*this->body.GetFrameLocalMatrix(this->frame_s.bodyb_f.first));
						this->body.SetFrameLocalMatrix(this->frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*this->body.GetFrameLocalMatrix(this->frame_s.body_f.first));
					}
				}
				else {
					m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
					this->body.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->pos - this->rec_HMD));
				}
			}
			//頭部
			if (this->HP != 0) {
				//MATRIX_ref mmm = this->body.GetFrameLocalMatrix(this->frame_s.body_f.first)*MATRIX_ref::Mtrans(this->body.GetFrameLocalMatrix(this->frame_s.body_f.first).pos()).Inverse();

				this->body.SetFrameLocalMatrix(this->frame_s.head_f.first, this->mat*m_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
				if (this->reloadf || this->key_.running || this->anime_arm_check->per == 1.f) {
					this->body.frame_reset(this->frame_s.head_f.first);
				}
			}
			//足
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

				easing_set(&this->ratio_r, ratio_t, 0.95f);
				if (this->HP != 0) {
					float ani_walk = 0.f;//1
					float ani_run = 0.f;//2
					float ani_sit = 0.f;//7
					float ani_wake = 0.f;//11
					float ani_swalk = 0.f;//8
					if (this->key_.running) {
						ani_run = 1.f*this->ratio_r;
					}
					else {
						if (!this->key_.squat.on()) {
							ani_wake = 1.f - this->ratio_r;
							ani_walk = this->ratio_r;
						}
						else {
							ani_sit = 1.f - this->ratio_r;
							ani_swalk = this->ratio_r;
						}
						if (!this->key_.ads.on()) {
							easing_set(&this->anime_hand_nomal->per, 0.f, 0.95f);
						}
					}

					//しゃがみ
					easing_set(&this->anime_sit->per, ani_sit, 0.95f);
					//立ち
					easing_set(&this->anime_wake->per, ani_wake, 0.95f);
					//走り
					easing_set(&this->anime_run->per, ani_run, 0.95f);
					this->anime_run->update(true, 1.f);
					//歩き
					easing_set(&this->anime_walk->per, ani_walk, 0.95f);
					this->anime_walk->update(true, 1.f);
					//しゃがみ歩き
					easing_set(&this->anime_swalk->per, ani_swalk, 0.95f);
					this->anime_swalk->update(true, ((this->anime_swalk->alltime / 30.f) / this->base.thisparts->reload_time));
				}
			}
			//視点
			{
				//エイム視点
				VECTOR_ref pv;
				if (this->base.thisparts->frame_s.site_f.first != INT_MAX) {
					pv = this->base.thisparts->frame_s.site_f.second;
				}
				for (auto&s : this->sight_) {
					if (s.attach) {
						pv = s.attach_frame.second + s.sight_parts.second;
						search_base(pv, &s);
					}
				}

				//gunpos指定
				if (this->key_.ads.on()) {
					easing_set(&this->gunpos, VGet(
						-pv.x(),
						-pv.y() + sin(DX_PI_F*2.f*(this->anime_walk->time / this->anime_walk->alltime))*this->anime_walk->per*0.001f,
						(this != &mine) ? -0.315f : -0.245f
					), 0.75f);
				}
				else if (this->key_.running) {
					easing_set(&this->gunpos, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f);
				}
				else {
					easing_set(&this->gunpos,
						VGet(
							-0.1f,
							-0.05f - pv.y() + sin(DX_PI_F*2.f*(this->anime_walk->time / this->anime_walk->alltime))*this->anime_walk->per*0.001f*2.f,
							(this != &mine) ? -0.335f : -0.275f
						), 0.75f);
				}
			}
			//手
			{
				this->body.frame_reset(this->frame_s.RIGHTarm1_f.first);
				this->body.frame_reset(this->frame_s.RIGHTarm2_f.first);
				this->body.frame_reset(this->frame_s.RIGHThand_f.first);
				this->body.frame_reset(this->frame_s.LEFTarm1_f.first);
				this->body.frame_reset(this->frame_s.LEFTarm2_f.first);
				this->body.frame_reset(this->frame_s.LEFThand_f.first);
				//
				if (this->HP != 0) {
					if (this->sort_ing) {
						this->anime_arm_check->per = 1.f;
						this->anime_arm_check->update(false, 1.f);
						if (this->anime_arm_check->time == this->anime_arm_check->alltime) {
							if (!CheckSoundMem(this->audio.sort_.get()) && !CheckSoundMem(this->audio.load_.get())) {
								this->sort_ing = false;
							}
						}
					}
					else {
						if (this->anime_arm_check->time == 0) {
							this->anime_arm_check->per = 0.f;
						}
						this->anime_arm_check->update(false, -1.f);
					}
					if (this->anime_arm_check->per == 1.f) {
						this->anime_arm_run->reset();
						this->anime_reload->reset();
						this->mat_gun = this->base.obj.GetMatrix()*MATRIX_ref::Mtrans(this->base.obj.GetMatrix().pos()).Inverse();
					}
					else {
						if (this->key_.running) {
							if (this->reloadf && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
								this->anime_reload->per = 1.f;
								this->anime_reload->update(true, ((this->anime_reload->alltime / 30.f) / this->base.thisparts->reload_time));

								this->anime_arm_run->reset();
							}
							else {
								this->anime_reload->reset();

								easing_set(&this->anime_arm_run->per, 1.f, 0.9f);
								this->anime_arm_run->update(true, 1.f);
							}
						}
						else {
							this->anime_arm_run->reset();
							if (this->reloadf && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
								this->anime_reload->per = 1.f;
								this->anime_reload->update(true, ((this->anime_reload->alltime / 30.f) / this->base.thisparts->reload_time));
							}
							else {
								this->anime_reload->reset();
								//右手
								{
									//視点を一時取得
									this->pos_HMD = (this->body.frame(this->frame_s.RIGHTeye_f.first) + (this->body.frame(this->frame_s.LEFTeye_f.first) - this->body.frame(this->frame_s.RIGHTeye_f.first))*0.5f) - this->pos;
									//銃器
									this->mat_gun = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->recoil_vec)*this->mat;//リコイル
									this->pos_gun = (this->pos + this->pos_HMD - this->rec_HMD) + (MATRIX_ref::Vtrans(this->gunpos, this->mat_gun) - this->rec_HMD);
									this->Set_gun();
									//
									move_righthand(false, m_inv, false);
								}
								//左手
								{
									{
										if (this->key_.down_mag) {
											this->pos_LEFTHAND = this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first) + this->mat_gun.yvec()*-0.05f;
										}
										else {
											if (this->foregrip.attach) {
												this->pos_LEFTHAND = this->foregrip.obj.frame(this->foregrip.LEFT_parts[0].first);
											}
											else {
												this->pos_LEFTHAND = this->base.obj.frame(this->base.thisparts->frame_s.left_f.first);
											}
										}
										if (!this->reloadf || !this->key_.down_mag) {
											this->LEFT_hand = true;
											if (this->foregrip.attach) {
												this->pos_LEFTHAND = this->foregrip.obj.frame(this->foregrip.LEFT_parts[0].first);
											}
											else {
												this->pos_LEFTHAND = this->base.obj.frame(this->base.thisparts->frame_s.left_f.first);
											}
										}
										else {
											this->LEFT_hand = false;
										}
										this->mat_LEFTHAND = this->mat;
									}
									move_lefthand(false, m_inv, false);
								}
							}
						}
					}
					//右人差し指
					{
						this->anime_hand_nomal->per = 1.f;
						this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
						this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					}
					this->addpos_gun = this->add_vec;
				}
				else {
					//銃器
					this->pos_gun += this->addpos_gun;
					this->addpos_gun.yadd(M_GR / std::powf(GetFPS(), 2.f));

					auto pp = MAPPTs->map_col_line(this->pos_gun + VGet(0, 1.f, 0), this->pos_gun - VGet(0, 0.05f, 0));
					if (pp.HitFlag) {
						this->pos_gun = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
						this->mat_gun *= MATRIX_ref::RotVec2(this->mat_gun.xvec(), VECTOR_ref(pp.Normal));
						easing_set(&this->addpos_gun, VGet(0, 0, 0), 0.8f);
					}
					this->Set_gun();
				}
			}
		}
		//
		void calc_gun() {
			if (this->HP != 0) {
				if (this->key_.running || (this->reloadf && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) || this->anime_arm_check->per == 1.f) {
					Set_gun2();
				}
				else {
					Set_gun3();
				}
			}
		}
		//
		void calc_cart(const float& cart_rate_t) {
			//弾
			this->bullet[this->use_bullet].Set(&this->base.thisparts->ammo[0], this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first), this->mat_gun.zvec()*-1.f);
			//薬莢
			this->cart[this->use_bullet].Set(&this->base.thisparts->ammo[0], this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first), (this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first + 1) - this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first)).Norm()*2.5f / GetFPS(), this->mat_gun, cart_rate_t);
			//
			++this->use_bullet %= this->bullet.size();//次のIDへ
		}
		//
		void calc_shot_effect() {
			if (this->mazzule.attach) {
				if (!this->mazzule.state1) {
					this->effcs[ef_fire].set(this->mazzule.obj.frame(this->mazzule.mazzule_frame.first), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);
				}
				else {
					this->effcs[ef_fire2].set(this->mazzule.obj.frame(this->mazzule.mazzule_frame.first), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);
				}
			}
			else {
				this->effcs[ef_fire].set(this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);
			}
		}
		//
		void update_effect(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs[0];
				if (index != ef_smoke) {
					t.put(DrawPts->get_effHandle((int32_t)(index)));
				}
			}
			for (auto& t : this->effcs_gndhit) {
				t.put(DrawPts->get_effHandle(ef_gndsmoke));
			}
		}
		//
		template<class Y2, class D2>
		void calc_shot(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts,
			std::vector<Chara>&chara,
			std::vector<Hit>& hit_obj,
			size_t& hit_buf,
			std::vector <Meds>& meds_data,
			std::vector<Items>& item
		) {
			const auto fps_ = GetFPS();
			if (this->gunanime_first->per == 1.f) {
				if ((this->gunanime_first->time > this->gunanime_first->alltime / 3.f) && (!this->audio.slide.check())) {
					this->audio.slide.play_3D(this->pos_gun, 15.f);
				}
				this->gunanime_first->update(false, 0.35f);
				if (this->gunanime_first->time >= this->gunanime_first->alltime) {
					this->gunanime_first->reset();
					this->gunanime_shot->per = 1.f;
				}
			}
			//複座
			easing_set(&this->recoil_vec, this->recoil_vec_res, 0.6f);
			easing_set(&this->recoil_vec_res, VGet(0, 0, 1.f), 0.95f);
			//リコイルアニメーション
			if (this->gunf) {
				if (this->gun_stat[this->base.thisparts->id].ammo_cnt >= 1) {
					this->gunanime_shot->per = 1.f;
					this->gunanime_shot->update(true, 2.f*this->gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
					if (this->gunanime_shot->time == 0.f) {
						this->gunf = false;
					}
					this->gunanime_shot_last->reset();
				}
				else {
					this->gunanime_shot_last->per = 1.f;
					this->gunanime_shot_last->update(false, 2.f*this->gunanime_shot_last->alltime / (FRAME_RATE / (600.f / 60.f)));
					if (this->gunanime_shot_last->time == 0.f) {
						this->gunf = false;
					}
					this->gunanime_shot->reset();
				}
			}
			//マガジン排出
			if (!this->reloadf && this->gunanime_magcatch->per >= 0.5f) {
				if (this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
					this->reloadf = true;
				}
				magrelease_t(item, 1);
			}
			if (!this->reloadf && this->gunanime_magcatch->per >= 0.5f && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
				this->reloadf = true;
				//音
				this->audio.mag_down.play_3D(this->pos_gun, 15.f);
				//弾数
				auto dnm = (this->gun_stat[this->base.thisparts->id].ammo_cnt >= 1) ? this->gun_stat[this->base.thisparts->id].ammo_cnt - 1 : 0;
				this->gun_stat[this->base.thisparts->id].mag_release();
				this->reload_cnt = 0.f;
				bool tt = false;
				for (auto& g : item) {
					if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
						tt = true;
						g.Set_item(this->magazine.thisparts, this->pos_mag, this->mat_mag);
						g.add = (this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
						g.magazine.cap = dnm;
						g.del_timer = 0.f;
						break;
					}
				}
				if (!tt) {
					item.resize(item.size() + 1);
					auto& g = item.back();
					g.id = item.size() - 1;
					g.Set_item(this->magazine.thisparts, this->pos_mag, this->mat_mag);
					g.add = (this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
					g.magazine.cap = dnm;
					g.del_timer = 0.f;
				}
			}
			//medkit生成
			if (this->key_.delete_item.push()) {
				bool tt = false;
				for (auto& g : item) {
					if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
						tt = true;
						g.Set_item(&meds_data[0], this->pos_mag, this->mat_mag);
						g.add = (this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first - 1) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first)).Norm()*-5.f / fps_;//排莢ベクトル
						break;
					}
				}
				if (!tt) {
					item.resize(item.size() + 1);
					auto& g = item.back();
					g.id = item.size() - 1;
					g.Set_item(&meds_data[0], this->pos_mag, this->mat_mag);
					g.add = (this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first - 1) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first)).Norm()*-5.f / fps_;//排莢ベクトル
				}
			}
			//マガジン整頓
			if (!this->sort_ing && this->key_.sortmag.push() && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 2) {
				this->sort_ing = true;
				if (!this->sort_f) {
					this->audio.sort_.play_3D(this->pos_gun, 15.f);
					std::sort(this->gun_stat[this->base.thisparts->id].mag_in.begin() + 1, this->gun_stat[this->base.thisparts->id].mag_in.end(), [](size_t a, size_t b) { return a > b;										});
					this->sort_f = true;
				}
				else {
					this->audio.load_.play_3D(this->pos_gun, 15.f);
					size_t siz = 0;
					for (auto& m : this->gun_stat[this->base.thisparts->id].mag_in) {
						if ((&m != &this->gun_stat[this->base.thisparts->id].mag_in[0]) && (m != this->magazine.thisparts->cap)) {
							siz = &m - &this->gun_stat[this->base.thisparts->id].mag_in[0];
							break;
						}
					}
					auto be_ = std::clamp<size_t>(this->gun_stat[this->base.thisparts->id].mag_in.back(), 0, this->magazine.thisparts->cap - this->gun_stat[this->base.thisparts->id].mag_in[siz]);
					this->gun_stat[this->base.thisparts->id].mag_in.back() -= be_;
					this->gun_stat[this->base.thisparts->id].mag_in[siz] += be_;

					magrelease_t(item, 2);
				}
			}
			//マガジン挿入
			if (this->reloadf && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
				if (DrawPts->use_vr && this->reload_cnt < this->base.thisparts->reload_time) {
					this->key_.down_mag = false;
				}
				if (this->key_.down_mag) {
					if (
						(DrawPts->use_vr) ?
						((this->magazine.obj.frame(this->magazine.magazine_f[1].first) - this->base.obj.frame(this->base.thisparts->frame_s.magazine_f.first)).size() <= 0.1f) :
						(this->reload_cnt > this->base.thisparts->reload_time)
						) {
						this->gunanime_shot->per = 1.f;
						this->gunanime_shot_last->reset();
						if (this->gun_stat[this->base.thisparts->id].ammo_cnt == 0) {
							this->gunanime_first->reset();
							this->gunanime_first->per = 1.f;
						}
						this->audio.mag_Set.play_3D(this->pos_gun, 15.f);
						this->gun_stat[this->base.thisparts->id].mag_slide();//チャンバーに装填
						this->reloadf = false;
					}
					this->pos_mag = this->pos_LEFTHAND;
					if (DrawPts->use_vr) {
						this->mat_mag =
							this->magazine.obj.GetFrameLocalMatrix(this->magazine.magazine_f[1].first) *
							this->magazine.obj.GetFrameLocalMatrix(this->magazine.magazine_f[0].first) *
							(
								this->mat_LEFTHAND*
								MATRIX_ref::RotVec2(
									this->mat_LEFTHAND.yvec(),
									this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first) - this->pos_LEFTHAND
								)
								);
					}
					else {
						this->mat_mag = this->mat_LEFTHAND;
					}
				}
				this->reload_cnt += 1.f / fps_;//挿入までのカウント
			}
			else {
				this->key_.down_mag = false;
				this->pos_mag = this->base.obj.frame(this->base.thisparts->frame_s.magazine2_f.first);
				this->mat_mag = this->mat_gun;
				//todo
				this->mat_mag = this->base.obj.GetMatrix()*MATRIX_ref::Mtrans(this->base.obj.GetMatrix().pos()).Inverse();
			}
			//セレクター
			easing_set(&this->gunanime_sel[1]->per, float(this->gun_stat[this->base.thisparts->id].select / std::max<size_t>(this->base.thisparts->select.size() - 1, 1)), 0.5f);
			easing_set(&this->gunanime_sel[0]->per, 1.f - this->gunanime_sel[1]->per, 0.5f);
			if (this->key_.selkey.push()) {
				++this->gun_stat[this->base.thisparts->id].select %= this->base.thisparts->select.size();
			}
			//射撃
			if (!this->gunf && this->gun_stat[this->base.thisparts->id].ammo_cnt >= 1) {
				if (this->base.thisparts->select[this->gun_stat[this->base.thisparts->id].select] == 2) {//フルオート用
					this->trigger.second = 0;
				}
			}
			this->trigger.get_in(this->gunanime_trigger->per >= 0.5f);
			if (this->trigger.push()) {
				if (this->mazzule.state1) {
					this->audio.trigger.vol(164);
				}
				else {
					this->audio.trigger.vol(255);
				}
				this->audio.trigger.play_3D(this->pos_gun, 5.f);
				if (!this->gunf && this->gun_stat[this->base.thisparts->id].ammo_cnt >= 1 && this->gunanime_first->per == 0.f) {
					this->gunf = true;
					//弾数管理
					this->gun_stat[this->base.thisparts->id].mag_shot(this->reloadf);
					//持ち手を持つとココが相殺される
					this->recoil_vec_res = MATRIX_ref::Vtrans(this->recoil_vec_res,
						MATRIX_ref::RotY(deg2rad(float((int32_t)(this->base.thisparts->recoil_xdn*100.f) + GetRand((int32_t)((this->base.thisparts->recoil_xup - this->base.thisparts->recoil_xdn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f))))*
						MATRIX_ref::RotX(deg2rad(float((int32_t)(this->base.thisparts->recoil_ydn*100.f) + GetRand((int32_t)((this->base.thisparts->recoil_yup - this->base.thisparts->recoil_ydn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f)))));
					//排莢、弾
					calc_cart(1.f);
					//エフェクト
					calc_shot_effect();
					//サウンド
					{
						if (this->mazzule.state1) {
							this->audio.shot.vol(192);
						}
						else {
							this->audio.shot.vol(255);
						}
						this->audio.shot.play_3D(this->pos_gun, 100.f);
					}
				}
			}
			this->magazine.obj.SetMatrix(this->mat_mag* MATRIX_ref::Mtrans(this->pos_mag));
			this->base.obj.work_anime();
			for (auto& a : this->bullet) {
				a.update(this, &chara, MAPPTs, hit_obj, hit_buf);
			}
			//薬莢の処理
			for (auto& a : this->cart) {
				a.get(MAPPTs, *this);
			}
			//
			update_effect(DrawPts);
			//
		}
		void Draw_gun() {
			//
			this->base.obj.DrawModel();
			for (auto& a : this->cart) {
				a.Draw();
			}
			//
			if (this->mazzule.attach) {
				this->mazzule.obj.DrawModel();
			}
			if (this->grip.attach) {
				this->grip.obj.DrawModel();
			}
			if (this->uperhandguard.attach) {
				this->uperhandguard.obj.DrawModel();
			}
			if (this->underhandguard.attach) {
				this->underhandguard.obj.DrawModel();
			}
			for (auto&m : mount_) {
				if (m.attach) {
					m.obj.DrawModel();
				}
			}
			if (this->dustcover.attach) {
				this->dustcover.obj.DrawModel();
			}
			if (this->stock.attach) {
				this->stock.obj.DrawModel();
			}
			if (this->foregrip.attach) {
				this->foregrip.obj.DrawModel();
			}
			if (this->light.attach) {
				this->light.obj.DrawModel();
			}
			if (this->laser.attach) {
				this->laser.obj.DrawModel();
			}
			for (auto&s : sight_) {
				if (s.attach) {
					s.obj.DrawModel();
				}
			}
			//
			if ((!this->reloadf || this->key_.down_mag) && this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1) {
				this->magazine.obj.DrawModel();
			}
		}
		void Draw_chara() {
			//
			this->body.DrawModel();
			//this->col.DrawModel();
			Draw_gun();
		}
		template<class Y2, class D2>
		void Draw_laser(std::vector<Chara>&chara, std::unique_ptr<Y2, D2>& MAPPTs, GraphHandle&light_pic) {
			if (this->laser.attach) {
				VECTOR_ref Startpos = this->laser.obj.frame(this->laser.source_parts.first);
				VECTOR_ref endpos = Startpos - this->base.obj.GetMatrix().zvec()*100.f;
				MAPPTs->map_col_nearest(Startpos, &endpos);
				for (auto& tgt : chara) {
					if (&tgt == this) {
						continue;
					}
					for (int i = 0; i < 3; i++) {
						auto q = tgt.col.CollCheck_Line(Startpos, endpos, -1, i);
						if (q.HitFlag) {
							endpos = q.HitPosition;
						}
					}
				}
				SetUseLighting(FALSE);
				SetFogEnable(FALSE);

				DXDraw::Capsule3D(Startpos, endpos, 0.001f, GetColor(255, 0, 0), GetColor(255, 255, 255));
				DrawSphere3D(endpos.get(), std::clamp(powf((endpos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.05f), 6, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
			}
			if (this->light.attach) {
				VECTOR_ref Startpos = this->light.obj.frame(this->laser.source_parts.first);

				DrawBillboard3D(Startpos.get(), 0.5f, 0.5f,
					0.1f,
					0.f, light_pic.get(), TRUE);
			}
		}
		void Set_Light() {
			if (this->light.attach) {
				VECTOR_ref Startpos = this->light.obj.frame(this->laser.source_parts.first);
				if (this->LightHandle == -1) {
					this->LightHandle = CreateSpotLightHandle(
						Startpos.get(),
						(this->base.obj.GetMatrix().zvec()*-1.f).get(),
						DX_PI_F / 16.0f, DX_PI_F / 24.0f, 25.0f, 0.1f, 0.35f, 0.0f);
				}
				SetLightPositionHandle(this->LightHandle, Startpos.get());
				SetLightDirectionHandle(this->LightHandle, (this->base.obj.GetMatrix().zvec()*-1.f).get());
			}
			else {
				if (this->LightHandle != -1) {
					DeleteLightHandle(this->LightHandle);
					this->LightHandle = -1;
				}
			}
		}
		void Draw_ammo() {
			for (auto& a : this->bullet) {
				a.Draw();
			}
		}
		void Delete_chara() {
			//パーツリセット
			delete_parts(EnumGunParts::PARTS_BASE);
			//
			this->body.Dispose();
			this->col.Dispose();
			//
			delete_parts(EnumGunParts::PARTS_MAGAZINE);
			//
			for (auto& a : this->cart) {
				a.delete_cart();
			}
			this->audio.Dispose();
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
			for (auto& t : this->effcs_gndhit) {
				t.handle.Dispose();
			}
			this->gun_stat.clear();
		}
		void operation_2_1(const bool& cannotmove, int32_t x_m, int32_t y_m) {
			if (cannotmove || this->HP == 0) {
				this->key_.Reset_();
			}
			this->key_.rule_();

			if (this->HP != 0) {
				//z軸回転(リーン)
				this->mat *= MATRIX_ref::RotAxis(this->mat.zvec(), this->body_zrad).Inverse();
				easing_set(&this->body_ztrun, -deg2rad(25 * x_m / 120)*1.f, 0.9f);
				if (this->key_.qkey) {
					easing_set(&this->body_zrad, deg2rad(-40), 0.9f);
				}
				else if (this->key_.ekey) {
					easing_set(&this->body_zrad, deg2rad(40), 0.9f);
				}
				else {
					easing_set(&this->body_zrad, this->body_ztrun, 0.8f);
				}
				this->mat *= MATRIX_ref::RotAxis(this->mat.zvec(), this->body_zrad);
				//y軸回転(旋回)
				this->mat = MATRIX_ref::RotX(-this->xrad_p)*this->mat;
				this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m)*0.1f, deg2rad(-80), deg2rad(60));
				this->mat *= MATRIX_ref::RotY(deg2rad(x_m)*0.1f);
				//x軸回転(仰俯)
				this->mat = MATRIX_ref::RotX(this->xrad_p)*this->mat;
			}


			this->speed = (this->key_.running ? 6.f : ((this->key_.ads.on() ? 2.f : 4.f)*(this->key_.squat.on() ? 0.4f : 1.f))) / GetFPS();
			VECTOR_ref zv_t = this->mat.zvec();
			zv_t.y(0.f);
			zv_t = zv_t.Norm();
			VECTOR_ref xv_t = this->mat.xvec();
			xv_t.y(0.f);
			xv_t = xv_t.Norm();
			if (this->key_.running) {
				xv_t = xv_t.Norm()*0.5f;
			}
			easing_set(&this->add_vec_buf,
				VECTOR_ref(VGet(0, 0, 0))
				+ (this->key_.wkey ? (zv_t*-this->speed) : VGet(0, 0, 0))
				+ (this->key_.skey ? (zv_t*this->speed) : VGet(0, 0, 0))
				+ (this->key_.akey ? (xv_t*this->speed) : VGet(0, 0, 0))
				+ (this->key_.dkey ? (xv_t*-this->speed) : VGet(0, 0, 0))
				, 0.95f);
			this->mat_body = (this->key_.running) ? MATRIX_ref::Axis1(this->mat.yvec().cross(this->add_vec_buf.Norm()*-1.f), this->mat.yvec(), this->add_vec_buf.Norm()*-1.f) : this->mat;
		}
		void operation_VR(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, const bool& cannotmove) {
			//操作
			{
				this->key_.aim = false;		//エイム(使わない)

				this->key_.shoot = false;	//射撃
				this->key_.reload = false;	//マグキャッチ
				this->key_.select = false;	//セレクター

				this->key_.get_ = false;	//アイテム取得
				this->key_.sort_ = false;	//
				this->key_.delete_ = false;	//
				this->key_.running = false;	//
				this->key_.jamp = false;	//jamp
				//
				if (this->HP != 0) {
					if (DrawPts->get_hand1_num() != -1) {
						auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hand1_num()];
						if (ptr_.turn && ptr_.now) {
							this->key_.shoot = ((ptr_.on[0] & BUTTON_TRIGGER) != 0);																				//射撃
							this->key_.reload = ((ptr_.on[0] & BUTTON_SIDE) != 0);																					//マグキャッチ
							this->key_.select = ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f&&ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f);	//セレクター
						}
					}
					if (DrawPts->get_hand2_num() != -1) {
						auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
							this->key_.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (this->gun_stat[this->base.thisparts->id].mag_in.size() >= 1));	//マガジン持つ
							this->key_.get_ = (ptr_.on[0] & BUTTON_TOPBUTTON_B) != 0;																	//アイテム取得
							this->key_.sort_ = false;																									//
							this->key_.delete_ = false;	//
							this->key_.running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;																	//running
							this->key_.jamp = (ptr_.on[0] & BUTTON_SIDE) != 0;																			//jamp
							//移動
							if (cannotmove || this->HP == 0) {
								easing_set(&this->add_vec_buf, VGet(0, 0, 0), 0.95f);
							}
							else {
								if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
									this->speed = (this->key_.running ? 8.f : 6.f) / GetFPS();

									if (DrawPts->tracker_num.size() > 0) {
										auto p = this->body.GetFrameLocalWorldMatrix(this->frame_s.bodyb_f.first);
										easing_set(&this->add_vec_buf, (p.zvec()*-ptr_.touch.y() + p.xvec()*-ptr_.touch.x())*this->speed, 0.95f);
									}
									else {
										easing_set(&this->add_vec_buf, (this->mat.zvec()*ptr_.touch.y() + this->mat.xvec()*ptr_.touch.x())*this->speed, 0.95f);
									}
								}
								else {
									easing_set(&this->add_vec_buf, VGet(0, 0, 0), 0.95f);
								}
							}
						}
					}
				}

				if (cannotmove || this->HP == 0) {
					this->key_.wkey = false;
					this->key_.skey = false;
					this->key_.akey = false;
					this->key_.dkey = false;
					this->key_.shoot = false;
					this->key_.running = false;
					this->key_.squat.first = false;
					this->key_.qkey = false;
					this->key_.ekey = false;
					this->key_.aim = false;
					this->key_.reload = false;
					this->key_.get_ = false;
					this->key_.sort_ = false;
					this->key_.delete_ = false;
					this->key_.select = false;
					this->key_.down_mag = true;
					this->key_.jamp = false;
				}
				//
			}
			//HMD_mat
			{
				//+視点取得
				auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hmd_num()];
				this->pos_HMD_old = this->pos_HMD;
				DrawPts->GetDevicePositionVR(DrawPts->get_hmd_num(), &this->pos_HMD, &this->mat);
				if (this->Start_c || (ptr_.turn && ptr_.now) != this->oldv_1_1) {
					this->rec_HMD = VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z());
				}
				this->oldv_1_1 = ptr_.turn && ptr_.now;
			}
		}
		void operation(const bool& cannotmove, const float& fov_per) {
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->HP != 0) {
				//操作
				this->key_.Set_(false);
				//
				GetMousePoint(&x_m, &y_m);
				x_m -= deskx / 2;
				y_m -= desky / 2;
				SetMousePoint(deskx / 2, desky / 2);
				SetMouseDispFlag(FALSE);
			}
			//
			x_m = int(float(std::clamp(x_m, -120, 120))*fov_per);
			y_m = int(float(std::clamp(y_m, -120, 120))*fov_per);

			operation_2_1(cannotmove, x_m, y_m);
		}
		template<class Y3, class D3>
		void operation_NPC(std::unique_ptr<Y3, D3>& MAPPTs, std::vector<Chara>&chara, const bool& cannotmove) {
			const auto fps_ = GetFPS();
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->HP != 0) {
				bool pp = true;
				bool is_player = false;
				//操作
				this->key_.aim = false;
				this->key_.reload = false;
				this->key_.get_ = false;
				this->key_.sort_ = false;
				this->key_.delete_ = false;
				this->key_.select = false;
				this->key_.down_mag = true;
				this->key_.shoot = false;
				this->key_.jamp = false;
				//AI
				VECTOR_ref vec_2 = chara[0].body.frame(chara[0].frame_s.bodyb_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first);
				if (this->ai_time_shoot < 0.f) {
					vec_2 = chara[0].body.frame(chara[0].frame_s.head_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first);
				}
				for (auto& tgt : chara) {
					if (this == &tgt || tgt.HP == 0) {
						continue;
					}
					auto tmp = tgt.body.frame(tgt.frame_s.bodyb_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first);
					if (this->ai_time_shoot < 0.f) {
						tmp = tgt.body.frame(tgt.frame_s.head_f.first) - this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first);
					}

					bool tt = true;
					{
						VECTOR_ref Startpos = this->base.obj.frame(this->base.thisparts->frame_s.mazzule_f.first);
						VECTOR_ref endpos = tgt.body.frame(tgt.frame_s.bodyb_f.first);
						if (this->ai_time_shoot < 0.f) {
							endpos = tgt.body.frame(tgt.frame_s.head_f.first);
						}
						auto p = MAPPTs->map_col_line(Startpos, endpos);
						if (p.HitFlag) {
							tt = false;
						}
					}
					if (!tt) {
						continue;
					}
					if (vec_2.size() >= tmp.size()) {
						vec_2 = tmp;
						is_player = (&tgt == &chara[0]);
						pp = false;
					}
				}
				{
					VECTOR_ref vec_x = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).xvec();
					VECTOR_ref vec_y = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).yvec();
					VECTOR_ref vec_z = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).zvec()*-1.f;
					auto ai_p_old = this->ai_phase;
					if (pp) {
						this->ai_phase = 0;
					}
					else {
						if (vec_z.dot(vec_2.Norm()) >= 0 && vec_2.size() <= 20.f) {
							this->ai_phase = 1;
						}
					}
					if (this->reloadf) {
						this->ai_phase = 2;
					}
					if ((ai_p_old == 1 && this->ai_phase != 1) || (this->add_vec_real.size() <= this->add_vec.size()*0.8f)) {
						int now = -1;
						auto poss = this->body.GetMatrix().pos();
						auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
						for (auto& w : MAPPTs->get_waypoint()) {
							auto id = &w - &MAPPTs->get_waypoint()[0];
							bool tt = true;
							for (auto& ww : this->wayp_pre) {
								if (id == ww) {
									tt = false;
								}
							}
							if (tt) {
								if (tmp.size() >= (w - poss).size()) {
									auto p = MAPPTs->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
									if (!p.HitFlag) {
										tmp = (w - poss);
										now = int(id);
									}
								}
							}
						}
						if (now != -1) {
							if (this->wayp_pre.size() > 0) {
								for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) {
									this->wayp_pre[i] = this->wayp_pre[i - 1];
								}
								this->wayp_pre[0] = now;
							}
						}
					}

					switch (this->ai_phase) {
					case 0://通常フェイズ
					{
						this->key_.running = false;
						this->key_.wkey = true;
						this->key_.skey = false;
						this->key_.akey = false;
						this->key_.dkey = false;
						this->key_.squat.first = false;
						this->key_.qkey = false;
						this->key_.ekey = false;

						this->ai_reload = false;
						this->ai_time_shoot = 0.f;

						vec_2 = MAPPTs->get_waypoint()[this->wayp_pre[0]] - this->body.GetMatrix().pos();
						x_m = -int(vec_x.dot(vec_2) * 120);
						y_m = -int(vec_y.dot(vec_2) * 120);

						//到達時に判断
						if (vec_2.size() <= 0.5f) {
							int now = -1;
							auto poss = this->body.GetMatrix().pos();
							auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
							for (auto& w : MAPPTs->get_waypoint()) {
								auto id = &w - &MAPPTs->get_waypoint()[0];
								bool tt = true;
								for (auto& ww : this->wayp_pre) {
									if (id == ww) {
										tt = false;
									}
								}
								if (tt) {
									if (tmp.size() >= (w - poss).size()) {
										auto p = MAPPTs->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
										if (!p.HitFlag) {
											tmp = (w - poss);
											now = int(id);
										}
									}
								}
							}
							if (now != -1) {
								if (this->wayp_pre.size() > 0) {
									for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) {
										this->wayp_pre[i] = this->wayp_pre[i - 1];
									}
									this->wayp_pre[0] = now;
								}
							}
						}
					}
					break;
					case 1://戦闘フェイズ
					{
						this->key_.running = false;
						this->key_.wkey = false;
						this->key_.skey = false;

						this->ai_reload = false;

						auto poss = this->body.GetMatrix().pos();
						if (this->key_.ekey) {
							this->ai_time_e += 1.f / fps_;
							if (this->ai_time_e >= 2) {
								this->key_.ekey = false;
								this->ai_time_e = 0.f;
							}
						}
						for (auto& w : MAPPTs->get_leanpoint_e()) {
							if ((w - poss).size() <= 0.5f) {
								this->key_.ekey = true;
								this->key_.qkey = false;
								this->ai_time_e = 0.f;
								break;
							}
						}
						if (this->key_.qkey) {
							this->ai_time_q += 1.f / fps_;
							if (this->ai_time_q >= 2) {
								this->key_.qkey = false;
								this->ai_time_q = 0.f;
							}
						}
						for (auto& w : MAPPTs->get_leanpoint_q()) {
							if ((w - poss).size() <= 0.5f) {
								this->key_.ekey = false;
								this->key_.qkey = true;
								this->ai_time_q = 0.f;
								break;
							}
						}

						int range = int(500.f + 500.f*vec_2.size() / 20.f);
						x_m = -int(vec_x.dot(vec_2) * 25) + int(float(-range + GetRand(range * 2)) / 100.f);
						y_m = -int(vec_y.dot(vec_2) * 25) + int(float(GetRand(range * 2)) / 100.f);

						this->ai_time_shoot += 1.f / fps_;
						if (this->ai_time_shoot < 0.f) {
							this->key_.akey = false;
							this->key_.dkey = false;
							this->key_.wkey = false;
							this->key_.skey = false;

							if (this->ai_time_shoot >= -5) {
								if (is_player) {
									this->key_.shoot = GetRand(100) <= 5;
								}
								else {
									this->key_.shoot = GetRand(100) <= 20;
								}
								this->key_.aim = true;
								this->key_.squat.first = true;
							}
						}
						else {
							this->key_.squat.first = false;
							if (is_player) {
								this->key_.shoot = GetRand(100) <= 10;
							}
							else {
								this->key_.shoot = GetRand(100) <= 30;
							}

							if (this->ai_time_shoot >= GetRand(20) + 5) {
								this->ai_time_shoot = float(-8);
							}
							if (this->key_.ekey && !this->key_.akey) {
								this->key_.akey = true;
								this->key_.dkey = false;
								this->ai_time_d = 0.f;
								this->ai_time_a = 0.f;
							}
							if (this->key_.qkey && !this->key_.dkey) {
								this->key_.dkey = true;
								this->key_.akey = false;
								this->ai_time_d = 0.f;
								this->ai_time_a = 0.f;
							}

							if (!this->key_.akey) {
								this->ai_time_d += 1.f / fps_;
								if (this->ai_time_d > GetRand(3) + 1) {
									this->key_.akey = true;
									this->ai_time_d = 0.f;
								}
							}
							else {
								this->ai_time_a += 1.f / fps_;
								if (this->ai_time_a > GetRand(3) + 1) {
									this->key_.akey = false;
									this->ai_time_a = 0.f;
								}
							}
							this->key_.dkey = !this->key_.akey;
						}
						if (this->gun_stat[this->base.thisparts->id].ammo_cnt == 0 && !this->reloadf) {
							this->key_.reload = true;
							this->ai_reload = true;
						}
					}
					break;
					case 2://リロードフェイズ
					{
						this->key_.wkey = true;
						this->key_.skey = false;
						this->key_.akey = false;
						this->key_.dkey = false;
						this->key_.running = true;
						this->key_.squat.first = false;
						this->key_.qkey = false;
						this->key_.ekey = false;

						if (this->ai_reload) {
							auto ppp = this->wayp_pre[1];
							for (int i = int(this->wayp_pre.size()) - 1; i >= 1; i--) {
								this->wayp_pre[i] = this->wayp_pre[0];
							}
							this->wayp_pre[0] = ppp;
							this->ai_reload = false;
						}
						auto poss = this->body.GetMatrix().pos();
						vec_2 = MAPPTs->get_waypoint()[this->wayp_pre[0]] - poss;
						x_m = -int(vec_x.dot(vec_2) * 40);
						y_m = -int(vec_y.dot(vec_2) * 40);

						//到達時に判断
						if (vec_2.size() <= 0.5f) {
							int now = -1;
							auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
							for (auto& w : MAPPTs->get_waypoint()) {
								auto id = &w - &MAPPTs->get_waypoint()[0];
								bool tt = true;
								for (auto& ww : this->wayp_pre) {
									if (id == ww) {
										tt = false;
									}
								}
								if (tt) {
									if (tmp.size() >= (w - poss).size()) {
										auto p = MAPPTs->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
										if (!p.HitFlag) {
											tmp = (w - poss);
											now = int(id);
										}
									}
								}
							}
							if (now != -1) {
								if (this->wayp_pre.size() > 0) {
									for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) {
										this->wayp_pre[i] = this->wayp_pre[i - 1];
									}
									this->wayp_pre[0] = now;
								}
							}
						}
					}
					break;
					default:
						break;
					}
				}
			}
			x_m = std::clamp(x_m, -120, 120);
			y_m = std::clamp(y_m, -120, 120);
			operation_2_1(cannotmove, x_m, y_m);
		}
		void operation_2() {
			//ジャンプ
			{
				if (this->add_ypos == 0.f) {
					if (this->key_.jamp && this->HP != 0) {
						this->add_ypos = 0.05f*FRAME_RATE / GetFPS();
					}
					this->add_vec = this->add_vec_buf;
				}
				else {
					easing_set(&this->add_vec, VGet(0, 0, 0), 0.995f);
				}
			}
			//操作
			{
				if (this->reloadf) {
					this->key_.aim = false;
				}
				//引き金(左クリック)
				easing_set(&this->gunanime_trigger->per, float(this->key_.shoot && !this->key_.running), 0.5f);
				//マグキャッチ(Rキー)
				easing_set(&this->gunanime_magcatch->per, float(this->key_.reload), 0.5f);
				//その他
				this->key_.update();
			}
		}
		template<class Y3, class D3>
		void Set_cam(std::unique_ptr<Y3, D3>& MAPPTs, cam_info& camera_main, std::vector<Chara>&chara, const float& fov_, const bool& use_vr) {
			if (this->HP != 0) {
				auto v_z = this->recoil_vec;
				v_z.y(v_z.y()*2.f / 3.f);
				v_z = v_z.Norm();
				auto mat_T = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), v_z)*this->mat;//リコイル
				if (use_vr) {
					mat_T = this->mat;
				}
				camera_main.set_cam_pos(this->pos + this->pos_HMD - this->rec_HMD, (this->pos + this->pos_HMD - this->rec_HMD) + mat_T.zvec()*(use_vr ? 1.f : -1.f), mat_T.yvec());
				if (this->key_.ads.on()) {
					easing_set(&camera_main.fov, deg2rad(25), 0.8f);
				}
				else {
					easing_set(&camera_main.fov, fov_, 0.9f);
				}
			}
			else {
				//デスカメラ
				easing_set(&camera_main.camvec, chara[this->death_id].pos + chara[this->death_id].pos_HMD - chara[this->death_id].rec_HMD, 0.9f);
				auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
				easing_set(&camera_main.campos, this->pos + this->pos_HMD - this->rec_HMD + VGet(-5.f*sin(rad), 2.f, -5.f*cos(rad)), 0.9f);
				camera_main.camup = VGet(0, 1.f, 0);
				MAPPTs->map_col_nearest(camera_main.camvec, &camera_main.campos);
				easing_set(&camera_main.fov, fov_, 0.9f);
			}
		}
		template<class Y3, class D3>
		void check_CameraViewClip(std::unique_ptr<Y3, D3>& MAPPTs) {
			auto ttt = this->body.GetMatrix().pos();
			if (CheckCameraViewClip_Box((ttt + VGet(-0.3f, 0, -0.3f)).get(), (ttt + VGet(0.3f, 1.8f, 0.3f)).get())) {
				this->Start_b = true;
				return;
			}
			auto p1 = ttt + VGet(0, 1.8f, 0);
			auto p2 = ttt + VGet(0, 0.f, 0);
			int cnt = 0;
			if (MAPPTs->map_col_line(GetCameraPosition(), p1).HitFlag) {
				cnt++;
			}
			if (MAPPTs->map_col_line(GetCameraPosition(), p2).HitFlag) {
				cnt++;
			}
			if (cnt == 2) {
				this->Start_b = true;
				return;
			}
		}
	};
	//アイテム
	class Items {
	private:
	public:
		//共通
		size_t id = 0;
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		//銃専用パラメーター
		gunparts* ptr_gun = nullptr;
		gunparts gun;
		//マガジン専用パラメーター
		gunparts* ptr_mag = nullptr;
		gunparts magazine;
		float del_timer = 0.f;
		//治療キット専用パラメーター
		Meds* ptr_med = nullptr;
		Meds medkit;
		//mag
		void Set_item(gunparts*magdata, const VECTOR_ref& pos_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add.clear();
			this->mat = mat_;
			//
			this->ptr_mag = magdata;
			this->obj = this->ptr_mag->mod.model.Duplicate();
		}
		//item
		void Set_item(Meds*meddata, const VECTOR_ref& pos_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add.clear();
			this->mat = mat_;
			//
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.model.Duplicate();
		}
		template<class Y, class D>
		void update(std::vector<Items>& item, std::unique_ptr<Y, D>& MAPPTs) {
			const auto fps_ = GetFPS();
			auto old = this->pos;

			old = this->pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr) {
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add;
				this->add.yadd(M_GR / powf(fps_, 2.f));
				for (auto& p : item) {
					if ((p.ptr_mag != nullptr || p.ptr_med != nullptr) && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.35f) {
							p.add.xadd((p.pos - this->pos).x()*5.f / fps_);
							p.add.zadd((p.pos - this->pos).z()*5.f / fps_);
							this->add.xadd((this->pos - p.pos).x()*5.f / fps_);
							this->add.zadd((this->pos - p.pos).z()*5.f / fps_);
						}
					}
				}
				auto pp = MAPPTs->map_col_line(old - VGet(0, 0.0025f, 0), this->pos - VGet(0, 0.0025f, 0));
				if (pp.HitFlag) {
					this->pos = VECTOR_ref(pp.HitPosition) + VECTOR_ref(pp.Normal)*0.005f;
					this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(pp.Normal)*-1.f);
					this->add.clear();
					//easing_set(&this->add, VGet(0, 0, 0), 0.8f);
				}
				//
			}
		}
		template<class Y2, class D2>
		void Get_item_2(Chara& chara, std::unique_ptr<Y2, D2>& MAPPTs) {
			if (this->ptr_mag != nullptr) {
				VECTOR_ref Startpos = chara.pos_LEFTHAND;
				VECTOR_ref endpos = Startpos - chara.mat_LEFTHAND.zvec()*2.6f;
				auto p = MAPPTs->map_col_line(Startpos, endpos);
				if (p.HitFlag) {
					endpos = p.HitPosition;
				}
				bool zz = false;
				{
					zz = (Segment_Point_MinLength(Startpos.get(), endpos.get(), this->pos.get()) <= 0.2f);
					chara.canget_magitem |= zz;
					if (zz) {
						chara.canget_id = this->id;
						chara.canget_mag = this->ptr_mag->mod.name;
						if (chara.key_.getmag.push() && this->magazine.cap != 0) {
							chara.sort_f = false;
							chara.gun_stat[this->ptr_mag->id].mag_plus(&(this->magazine));
							if (chara.gun_stat[this->ptr_mag->id].mag_in.size() == 1) {
								chara.reloadf = true;
							}
							this->Delete_item();
						}
					}
				}
			}

			if (this->ptr_med != nullptr) {
				VECTOR_ref Startpos = chara.pos_LEFTHAND;
				VECTOR_ref endpos = Startpos - chara.mat_LEFTHAND.zvec()*2.6f;
				auto p = MAPPTs->map_col_line(Startpos, endpos);
				if (p.HitFlag) {
					endpos = p.HitPosition;
				}
				bool zz = false;
				{
					zz = (Segment_Point_MinLength(Startpos.get(), endpos.get(), this->pos.get()) <= 0.2f);
					chara.canget_meditem |= zz;
					if (zz) {
						chara.canget_id = this->id;
						chara.canget_med = this->ptr_med->mod.name;
						if (chara.key_.getmag.push()) {
							chara.HP = std::clamp<int>(chara.HP + this->ptr_med->repair, 0, chara.HP_full);
							//ITEM
							this->Delete_item();
						}
					}
				}
			}
			//個別
			if (this->ptr_mag != nullptr && this->magazine.cap == 0) {
				this->del_timer += 1.f / GetFPS();
			}
		}
		void Draw_item() {
			if (this->ptr_gun != nullptr) {
				this->obj.DrawModel();
			}
			if (this->ptr_mag != nullptr) {
				this->obj.DrawModel();
			}
			if (this->ptr_med != nullptr) {
				this->obj.DrawModel();
			}
		}
		void Draw_item(Chara& chara) {
			if (this->ptr_gun != nullptr) {
				if (chara.canget_gunitem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
			if (this->ptr_mag != nullptr) {
				if (chara.canget_magitem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
			if (this->ptr_med != nullptr) {
				if (chara.canget_meditem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
		}
		void Delete_item() {
			this->ptr_gun = nullptr;
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->obj.Dispose();
		}
	};
};
//
