#pragma once
#define FRAME_RATE 90.f
//リサイズ
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)
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
	UPER_RAIL,
	UNDER_RAIL,
	LEFTSIDE_RAIL,
	RIGHTSIDE_RAIL,
	SIDEMOUNT,
	SIDEMOUNT_BASE,
	STOCK_BASE,
	DUSTCOVER_BASE,
	UPER_HANDGUARD,
	UNDER_HANDGUARD,
	MAZZULE_BASE,
	MAGAZINE_BASE,
	GRIP_BASE,
	NUM_,
};
//option
class OPTION {
public:
	bool DoF = false;
	bool Bloom = false;
	bool Shadow = false;
	bool useVR = true;
	bool SSAO = true;
	float Fov = 45.f;
	bool Vsync = false;
	OPTION(void) {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		DoF = getparams::_bool(mdata);
		Bloom = getparams::_bool(mdata);
		Shadow = getparams::_bool(mdata);
		useVR = getparams::_bool(mdata);
		SSAO = getparams::_bool(mdata);
		Fov = getparams::_float(mdata);
		Vsync = getparams::_bool(mdata);
		FileRead_close(mdata);
		SetOutApplicationLogValidFlag(TRUE);
	}
	~OPTION(void) {
	}
};
//キーバインド
class key_bind {
private:
	struct keyhandle;
	struct keys {
		int mac = 0, px = 0, py = 0;
		char onhandle[256]="", offhandle[256]="";
	};
	class key_pair {
	public:
		int first = 0;
		std::string second;
		bool isalways = false;
		switchs on_off;
		keyhandle* use_handle = nullptr;
		bool get_key(const int& id) {
			switch (id) {
				//キー
			case 0:
				return CheckHitKey(this->first) != 0;
			case 1:
				on_off.get_in(CheckHitKey(this->first) != 0);
				return on_off.on();
			case 2:
				on_off.get_in(CheckHitKey(this->first) != 0);
				return on_off.push();
				//マウス
			case 3:
				return (GetMouseInput() & this->first) != 0;
			case 4:
				on_off.get_in((GetMouseInput() & this->first) != 0);
				return on_off.on();
			case 5:
				on_off.get_in((GetMouseInput() & this->first) != 0);
				return on_off.push();
			default:
				return CheckHitKey(this->first) != 0;
			}
		}
	};
	struct keyhandle {
		keys key;
		GraphHandle onhandle, offhandle;
		key_pair* use_part = nullptr;
	};
	FontHandle font24;
	int font24size = 24;
	std::vector<keyhandle> keyg;
	std::vector<keyhandle> keyg2;

	float F1_f = 0.0f;
	GraphHandle mousehandle;
public:
	std::vector < key_pair > key_use_ID;
	std::vector < key_pair > mouse_use_ID;
	//
	key_bind() {
		SetUseASyncLoadFlag(FALSE);
		font24 = FontHandle::Create(font24size, DX_FONTTYPE_EDGE);
		mousehandle = GraphHandle::Load("data/key/mouse.png");
		//
		{
			key_pair tmp_k;
			tmp_k.first = KEY_INPUT_W;
			tmp_k.second = "前進";
			this->key_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = KEY_INPUT_S;
			tmp_k.second = "後退";
			this->key_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = KEY_INPUT_D;
			tmp_k.second = "右歩き";
			this->key_use_ID.emplace_back(tmp_k);//2
			tmp_k.first = KEY_INPUT_A;
			tmp_k.second = "左歩き";
			this->key_use_ID.emplace_back(tmp_k);//3
			tmp_k.first = KEY_INPUT_Q;
			tmp_k.second = "左リーン";
			this->key_use_ID.emplace_back(tmp_k);//4
			tmp_k.first = KEY_INPUT_E;
			tmp_k.second = "右リーン";
			this->key_use_ID.emplace_back(tmp_k);//5
			tmp_k.first = KEY_INPUT_R;
			tmp_k.second = "リロード";
			this->key_use_ID.emplace_back(tmp_k);//6
			tmp_k.first = KEY_INPUT_F;
			tmp_k.second = "アイテム取得";
			this->key_use_ID.emplace_back(tmp_k);//7
			tmp_k.first = KEY_INPUT_G;
			tmp_k.second = "治療キット排出";
			this->key_use_ID.emplace_back(tmp_k);//8
			tmp_k.first = KEY_INPUT_C;
			tmp_k.second = "しゃがみ";
			this->key_use_ID.emplace_back(tmp_k);//9
			tmp_k.first = KEY_INPUT_O;
			tmp_k.second = "タイトル画面に戻る";
			this->key_use_ID.emplace_back(tmp_k);//10
			tmp_k.first = KEY_INPUT_ESCAPE;
			tmp_k.second = "強制終了";
			this->key_use_ID.emplace_back(tmp_k);//11
			tmp_k.first = KEY_INPUT_Z;
			tmp_k.second = "マガジン整理";
			this->key_use_ID.emplace_back(tmp_k);//12
			tmp_k.first = KEY_INPUT_LSHIFT;
			tmp_k.second = "走る";
			this->key_use_ID.emplace_back(tmp_k);//13
			tmp_k.first = KEY_INPUT_SPACE;
			tmp_k.second = "ジャンプ";
			this->key_use_ID.emplace_back(tmp_k);//14
			tmp_k.first = KEY_INPUT_LCONTROL;
			tmp_k.second = "視点切替";
			this->key_use_ID.emplace_back(tmp_k);//15
			tmp_k.first = KEY_INPUT_F1;
			tmp_k.second = "キー案内";
			this->key_use_ID.emplace_back(tmp_k);//16
			//
			tmp_k.first = MOUSE_INPUT_LEFT;
			tmp_k.second = "射撃";
			this->mouse_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = MOUSE_INPUT_MIDDLE;
			tmp_k.second = "セレクター切替";
			this->mouse_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = MOUSE_INPUT_RIGHT;
			tmp_k.second = "エイム";
			this->mouse_use_ID.emplace_back(tmp_k);//2
		}
		{
			std::fstream file;
			/*
			std::vector<keys> key;
			key.resize(key.size() + 1);
			key.back().mac = MOUSE_INPUT_LEFT;
			key.back().px=0;
			key.back().py=0;
			strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/LEFT.bmp");
			strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/LEFT.bmp");
			key.resize(key.size() + 1);
			key.back().mac = MOUSE_INPUT_MIDDLE;
			key.back().px = 0;
			key.back().py = 0;
			strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/MIDDLE.bmp");
			strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/MIDDLE.bmp");
			key.resize(key.size() + 1);
			key.back().mac = MOUSE_INPUT_RIGHT;
			key.back().px = 0;
			key.back().py = 0;
			strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/RIGHT.bmp");
			strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/RIGHT.bmp");


			file.open("data/2.dat", std::ios::binary | std::ios::out);
			for (auto& m : key) {
				file.write((char*)&m, sizeof(m));
			}
			file.close();
			//*/
			//*
			{
				file.open("data/1.dat", std::ios::binary | std::ios::in);
				keys keytmp;
				while (true) {
					file.read((char*)&keytmp, sizeof(keytmp));
					if (file.eof()) {
						break;
					}
					int y_size = 48;//36
					{
						this->keyg.resize(this->keyg.size() + 1);
						this->keyg.back().key = keytmp;

						keyg.back().key.px = keyg.back().key.px * y_size + ((keyg.back().key.px <= 14 && keyg.back().key.py % 2 == 1) ? y_size / 2 : 0) + ((keyg.back().key.px > 14) ? y_size : 0);
						keyg.back().key.py = keyg.back().key.py * y_size;
						this->keyg.back().onhandle = GraphHandle::Load(this->keyg.back().key.onhandle);
						this->keyg.back().offhandle = GraphHandle::Load(this->keyg.back().key.offhandle);
					}
				}
				for (auto& m : this->keyg) {
					for (auto& i : this->key_use_ID) {
						if (i.first == m.key.mac) {
							m.use_part = &i;
							i.use_handle = &m;
							break;
						}
					}
				}
				file.close();
			}
			{
				file.open("data/2.dat", std::ios::binary | std::ios::in);
				int cnt = 0;
				keys keytmp;
				while (true) {
					file.read((char*)&keytmp, sizeof(keytmp));
					if (file.eof()) {
						break;
					}
					int y_size = 48;//36
					{
						this->keyg2.resize(this->keyg2.size() + 1);
						this->keyg2.back().key = keytmp;
						keyg2.back().key.px = cnt * y_size;
						keyg2.back().key.py = 500;
						cnt++;

						this->keyg2.back().onhandle = GraphHandle::Load(this->keyg2.back().key.onhandle);
						this->keyg2.back().offhandle = GraphHandle::Load(this->keyg2.back().key.offhandle);
					}
				}
				for (auto& m : this->keyg2) {
					for (auto& i : this->mouse_use_ID) {
						if (i.first == m.key.mac) {
							m.use_part = &i;
							i.use_handle = &m;
							break;
						}
					}
				}
				file.close();
			}
			//*/
		}
	}
	//
	const auto Esc_key() { return this->key_use_ID[11].get_key(0); }
	//
	void reSet_isalways() {
		for (auto& i : this->key_use_ID) {
			i.isalways = false;
		}
		for (auto& i : this->mouse_use_ID) {
			i.isalways = false;
		}
		this->key_use_ID[11].isalways = true;
		this->key_use_ID[16].isalways = true;
	}
	//
	void draw() {
		auto tmp_f1 = this->key_use_ID[16].get_key(1);
		easing_set(&F1_f, float(tmp_f1), 0.9f);
		//インフォ
		if (F1_f > 0.1f) {
			int xp_t = 100, yp_t = 300;
			int xp_sk = xp_t, yp_sk = yp_t, y_size_k = 48;
			int xp_s = 1500, yp_s = 200, y_size = 32;
			//背景
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*F1_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			//前面
			if (F1_f > 0.9f) {
				//キーボード＋マウス全部
				{
					bool use = true;
					for (auto& m : this->keyg) {
						use = true;
						if (m.use_part != nullptr) {
							use = false;
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							{
								xp_sk = xp_t + m.key.px;
								yp_sk = yp_t + m.key.py;
								if (m.use_part->get_key(0)) {
									m.onhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - 4) / 26.f, 0.f, false);
								}
								else {
									m.offhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - 4) / 26.f, 0.f, false);
								}
							}
						}
						if (use) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
							xp_sk = xp_t + m.key.px;
							yp_sk = yp_t + m.key.py;
							m.offhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - 4) / 26.f, 0.f, false);
						}
					}
					int xss = 0, yss = 0;
					int y_size_k2 = 256;
					xp_sk = 1100;
					yp_sk = 800;
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					{
						mousehandle.GetSize(&xss, &yss);
						mousehandle.DrawRotaGraph(xp_sk, yp_sk, float(y_size_k2) / yss, 0.f, true);
					}
					for (auto& m : this->keyg2) {
						use = true;
						if (m.use_part != nullptr) {
							use = false;
							if (m.use_part->get_key(3)) {
								m.onhandle.GetSize(&xss, &yss);
								m.onhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_size_k2) / yss, 0.f, true);
							}
							else {
								m.offhandle.GetSize(&xss, &yss);
								m.offhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_size_k2) / yss, 0.f, true);
							}
						}
						if (use) {
							xp_sk = xp_t;
							yp_sk = yp_t;
							m.offhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_size_k2 - 4) / 26.f, 0.f, true);
						}
					}
				}
				//詳細
				int xss = 0, yss = 0;
				for (auto& i : this->key_use_ID) {
					if (i.isalways && i.use_handle != nullptr) {
						if (i.get_key(0)) {
							i.use_handle->onhandle.GetSize(&xss, &yss);
							xss = int(float(xss)*float(y_size - 4) / 25.f);
							yss = int(float(yss)*float(y_size - 4) / 25.f);
							i.use_handle->onhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, float(y_size - 4) / 25.f, 0.f, false);
						}
						else {
							i.use_handle->offhandle.GetSize(&xss, &yss);
							xss = int(float(xss)*float(y_size - 4) / 25.f);
							yss = int(float(yss)*float(y_size - 4) / 25.f);
							i.use_handle->offhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, float(y_size - 4) / 25.f, 0.f, false);
						}
						font24.DrawString(xp_s, yp_s + (y_size - font24size) / 2, i.second, GetColor(255, 255, 255)); yp_s += y_size;
					}
				}
				for (auto& i : this->mouse_use_ID) {
					if (i.isalways && i.use_handle != nullptr) {
						{
							mousehandle.GetSize(nullptr, &yss);
							mousehandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
						}
						if (i.get_key(3)) {
							i.use_handle->onhandle.GetSize(nullptr, &yss);
							i.use_handle->onhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
						}
						else {
							i.use_handle->offhandle.GetSize(nullptr, &yss);
							i.use_handle->offhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
						}
						font24.DrawString(xp_s, yp_s + (y_size - font24size) / 2, i.second, GetColor(255, 255, 255)); yp_s += y_size;
					}
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		//常時表示
		if (!tmp_f1) {
			int xp_s = 1920 - 700, yp_s = 1080 - 28, x_size = 26, y_size = 24;
			int xss = 0, yss = 0;
			for (auto& i : this->key_use_ID) {
				if (i.isalways) {
					for (auto& m : this->keyg) {
						if (m.key.mac == i.first) {
							if (i.get_key(0)) {
								m.onhandle.GetSize(&xss, &yss);
								xss = int(float(xss)*float(y_size - 4) / 25.f);
								yss = int(float(yss)*float(y_size - 4) / 25.f);
								m.onhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, float(y_size - 4) / 25.f, 0.f, false);
							}
							else {
								m.offhandle.GetSize(&xss, &yss);
								xss = int(float(xss)*float(y_size - 4) / 25.f);
								yss = int(float(yss)*float(y_size - 4) / 25.f);
								m.offhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, float(y_size - 4) / 25.f, 0.f, false);
							}
							xp_s += (x_size - y_size + xss);
						}
					}
				}
			}
			for (auto& i : this->mouse_use_ID) {
				if (i.isalways) {
					for (auto& m : this->keyg2) {
						if (m.key.mac == i.first) {
							{
								mousehandle.GetSize(&xss, &yss);
								mousehandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
							}
							if (i.get_key(3)) {
								m.onhandle.GetSize(&xss, &yss);
								m.onhandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
							}
							else {
								m.offhandle.GetSize(&xss, &yss);
								m.offhandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
							}
							xp_s += x_size;
						}
					}
				}
			}
			//
		}
		//
	}
	//
};
//ルール
class RULE_parts {
private:
	float Ready = 0.f;
	float timer = 0.f;
public:
	float get_timer(void) { return timer; }
	float get_Ready(void) { return Ready; }
	bool get_Start(void) { return Ready <= 0.f; }
	bool get_Playing(void) { return get_Start() && !get_end(); }
	bool get_end(void) { return timer <= 0.f; }
	void Set(void) {
		Ready = 3.0f;
		timer = 180.f;
	}
	void UpDate(void) {
		if (get_Start()) {
			timer -= 1.f / GetFPS();
		}
		else {
			Ready -= 1.f / GetFPS();
		}
	}
};
//
class save_c {
public:
	size_t cang_ = 0;					//パーツ選択
	size_t type_ = 0;					//パーツの種類
	size_t pt_cat_ = SIZE_MAX;			//ベースパーツの場所
	size_t pt_type_ = 0;				//ベースパーツの種類
	size_t pt_sel_ = 0;					//ベースパーツの番号(マウントなど)
};
//
class Mainclass {
private:
	//銃、マガジン共通モデル
	class Models {
	private:
		std::string name;
		std::string path;
		MV1 model;
		MV1 col;
	public:
		auto& get_name() { return name; }
		auto& get_path() { return path; }
		auto& get_model() { return model; }

		void Ready(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/model.mv1", &this->model, true);
			MV1::Load(this->path + "/col.mv1", &this->col, true);
		}
		int mdata = -1;
		void Set_(std::function<void()> doing) {
			this->model.SetMatrix(MGetIdent());
			this->mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			doing();
			FileRead_close(mdata);
		}
	};
protected:
	//銃用オーディオ
	class Audios {
	private:
	public:
		std::string shot_path;
		SoundHandle shot;
		std::string slide_path;
		SoundHandle slide;
		std::string trigger_path;
		SoundHandle trigger;
		SoundHandle magazine_down;
		SoundHandle magazine_Set;
		SoundHandle case_down;
		SoundHandle load_;
		SoundHandle sort_magazine;
		SoundHandle foot_;
		//
		SoundHandle voice_damage;
		SoundHandle voice_death;

		void Set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot_path = "data/audio/shot_" + getparams::_str(mdata) + ".wav";
			shot = SoundHandle::Load(shot_path);
			slide_path = "data/audio/slide_" + getparams::_str(mdata) + ".wav";
			slide = SoundHandle::Load(slide_path);
			trigger_path = "data/audio/trigger_" + getparams::_str(mdata) + ".wav";
			trigger = SoundHandle::Load(trigger_path);
			magazine_down = SoundHandle::Load("data/audio/magazine_down_" + getparams::_str(mdata) + ".wav");
			magazine_Set = SoundHandle::Load("data/audio/magazine_Set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_2.wav");
			load_ = SoundHandle::Load("data/audio/load.wav");
			sort_magazine = SoundHandle::Load("data/audio/sort.wav");
			foot_ = SoundHandle::Load("data/audio/foot_sand.wav");
			voice_damage = SoundHandle::Load("data/audio/voice/damage.wav");
			voice_death = SoundHandle::Load("data/audio/voice/death.wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);

		}
		void Duplicate(Audios& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->shot_path = tgt.shot_path;
			this->slide_path = tgt.slide_path;
			this->trigger_path = tgt.trigger_path;
			this->shot = tgt.shot.Duplicate();
			this->slide = tgt.slide.Duplicate();
			this->trigger = tgt.trigger.Duplicate();
			this->magazine_down = tgt.magazine_down.Duplicate();
			this->magazine_Set = tgt.magazine_Set.Duplicate();
			this->load_ = tgt.load_.Duplicate();
			this->sort_magazine = tgt.sort_magazine.Duplicate();
			this->foot_ = tgt.foot_.Duplicate();
			this->voice_damage = tgt.voice_damage.Duplicate();
			this->voice_death = tgt.voice_death.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void Dispose(void) {
			this->shot_path = "";
			this->slide_path = "";
			this->trigger_path = "";
			this->shot.Dispose();
			this->slide.Dispose();
			this->trigger.Dispose();
			this->magazine_down.Dispose();
			this->magazine_Set.Dispose();
			this->load_.Dispose();
			this->sort_magazine.Dispose();
			this->foot_.Dispose();
			this->voice_damage.Dispose();
			this->voice_death.Dispose();
		}
	};
	//弾データ
	class Ammos {
	private:
		std::string name;
		std::string path;
		MV1 model;
		//
		MV1 model_full;//未発射
		//
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		int damage = 10;//ダメージ
	public:
		auto& get_model() { return model; }//this->base.thisparts->ammo[0].get_model()
		auto& get_model_full() { return model_full; }
		auto& get_caliber() { return caliber; }
		auto& get_speed() { return speed; }
		auto& get_pene() { return pene; }
		auto& get_damage() { return damage; }
		auto& get_name() { return name; }

		void Set_before(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/ammo.mv1", &this->model, true);
			MV1::Load(this->path + "/ammo2.mv1", &this->model_full, true);
		}
		void Set(void) {
			int mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_int(mdata);//ダメージ
			FileRead_close(mdata);
		}
	};
	//パフォーマンス
	struct performance {
		float recoil = 0.f;
		float weight = 0.f;
		std::string name;
		std::string info;

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
	class GUNPARTs {
		int type = 0;
	public:
		//base
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//再装填時間
		//
		std::vector <uint8_t> select;	//セレクター
		//common
		size_t id_t = 0;
		Models mod;
		performance per;
		//mazzule
		size_t mazzule_type = 0;
		//sight
		GraphHandle reticle;
		//stock
		size_t stock_type = 0;
		//
		std::vector<std::string> can_attach;
		std::vector<Ammos> ammo;
		Audios audio;
		//magazine
		size_t cap = 1;
		//
		auto& get_type() { return type; }
		void Set_datas(const size_t& id_, const int& type_t) {
			this->id_t = id_;
			this->type = type_t;
			//テキスト
			this->mod.Set_([&]() {
				//共通データ
				{
					per.name = getparams::get_str(this->mod.mdata);		//名前
					per.info = getparams::get_str(this->mod.mdata);		//説明
				}
				//
				if (this->type == EnumGunParts::PARTS_MAZZULE) {
					mazzule_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == EnumGunParts::PARTS_STOCK) {
					stock_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == EnumGunParts::PARTS_BASE) {
					//セレクター設定
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
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
					this->audio.Set(this->mod.mdata);						//サウンド
					this->recoil_xup = getparams::_float(this->mod.mdata);	//反動
					this->recoil_xdn = getparams::_float(this->mod.mdata);	//反動
					this->recoil_yup = getparams::_float(this->mod.mdata);	//反動
					this->recoil_ydn = getparams::_float(this->mod.mdata);	//反動
					this->reload_time = getparams::_float(this->mod.mdata);	//リロードタイム
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				if (this->type == EnumGunParts::PARTS_MAGAZINE) {
					this->cap = getparams::_long(this->mod.mdata);			//口径
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				//共通データ
				{
					per.recoil = getparams::_float(this->mod.mdata);		//リコイル
					per.weight = getparams::_float(this->mod.mdata);		//重量
					{
						can_attach.clear();
						while (FileRead_eof(this->mod.mdata) == 0) {
							can_attach.resize(can_attach.size() + 1);
							can_attach.back() = getparams::get_str(this->mod.mdata);
						}
					}
				}
			});
			if (this->type == EnumGunParts::PARTS_SIGHT) {
				SetUseASyncLoadFlag(FALSE);
				this->reticle = GraphHandle::Load(this->mod.get_path() + "/reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
			if (this->type == EnumGunParts::PARTS_BASE) {
				//フレーム
				for (auto& a : this->ammo) {
					a.Set();
				}
			}
		}
		//
		void Set_Ammos_data(const int& mdata) {
			while (true) {
				auto p = getparams::_str(mdata);
				if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
					break;
				}
				else {
					this->ammo.resize(this->ammo.size() + 1);
					this->ammo.back().Set_before("data/ammo/", p);
				}
			}
		}
		//
		int Select_Chose(const int& sel_chose) {
			for (auto& s : this->select) {
				if (s == sel_chose) {//1~4
					return int(&s - &this->select[0]);
				}
			}
			return -1;
		}
		//
		void Set_gun_select(std::vector<MV1::ani*>&gunanime_sel, const int& get_select) {
			for (auto& sel : this->select) {
				easing_set(&gunanime_sel[&sel - &this->select[0]]->per, float(int(sel == this->select[get_select])), 0.5f);
			}
		}
	};
	//薬品データ
	class Meds {
	private:
	public:
		size_t id_t = 0;
		Models mod;
		/**/
		int repair = 0;
		void Set_datas(const size_t& id_) {
			this->id_t = id_;
			//テキスト
			this->mod.Set_([&]() {
				this->repair = getparams::_long(this->mod.mdata);//
			});
		}
	};
	//命中根
	class Hit {
	private:
		bool Flag = false;	/*弾痕フラグ*/
		int use = 0;		/*使用フレーム*/
		MV1 pic;			/*弾痕モデル*/
		VECTOR_ref pos;		/*座標*/
		MATRIX_ref mat;		/*座標*/
	public:
		void Set(const MV1&hit_pic) {
			this->Flag = false;
			this->use = 0;
			this->pic = hit_pic.Duplicate();
			this->mat.clear();
			this->pos.clear();
		}
		void Put_Hit(const float &caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			float asize = 200.f*caliber;
			VECTOR_ref y_vec = Normal;
			VECTOR_ref z_vec = y_vec.cross(Zvec).Norm();
			VECTOR_ref scale = VGet(asize / std::abs(y_vec.dot(Zvec)), asize, asize);

			this->Flag = true;
			this->use = 0;
			this->pos = VECTOR_ref(Position) + y_vec * 0.02f;
			this->mat = MATRIX_ref::Scale(scale)*  MATRIX_ref::Axis1_YZ(y_vec, z_vec);
		}
		void UpDate() {
			if (this->Flag) {
				this->pic.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
			}
		}
		void Draw(void) {
			if (this->Flag) {
				this->pic.DrawFrame(this->use);
			}
		}
	};
	//アイテム
	class Items {
	private:
		//共通
		VECTOR_ref pos;
		VECTOR_ref add_vec;
		MATRIX_ref mat;
		MV1 obj;
		//マガジン専用パラメーター
		//治療キット専用パラメーター
	private:
		//共通
		float del_timer = 0.f;
		size_t id_t = 0;
		//マガジン専用パラメーター
		GUNPARTs* ptr_mag = nullptr;
		GUNPARTs magazine;
		//治療キット専用パラメーター
		Meds* ptr_med = nullptr;
		Meds medkit;
	public:
		auto& get_ptr_mag() { return ptr_mag; }
		auto& get_ptr_med() { return ptr_med; }
		auto& get_magazine() { return magazine; }
		auto& get_pos_() { return pos; }
	private:
		//mag
		void Set_item_1(GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add_vec = add_;
			this->mat = mat_;
			this->ptr_mag = magdata;
			this->obj = this->ptr_mag->mod.get_model().Duplicate();
		}
		//med
		void Set_item(Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add_vec = add_;
			this->mat = mat_;
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.get_model().Duplicate();
		}
	public:
		//mag
		void Set_item_before(const size_t& id, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->id_t = id;
			this->Set_item_1(magdata, pos_, add_, mat_);
			if (this->ptr_mag != nullptr) {
				this->magazine.cap = int(this->ptr_mag->cap);
				if (this->magazine.ammo.size() < this->ptr_mag->ammo.size()) {
					this->magazine.ammo.resize(this->ptr_mag->ammo.size());
				}
				this->magazine.ammo[0].get_name() = this->ptr_mag->ammo[0].get_name();
			}
		}
		void Set_item_magrelease_(const size_t& dnm, const size_t& id, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->id_t = id;
			this->Set_item_1(magdata, pos_, add_, mat_);
			this->magazine.cap = dnm;
			if (this->ptr_mag != nullptr) {
				if (this->magazine.ammo.size() < this->ptr_mag->ammo.size()) {
					this->magazine.ammo.resize(this->ptr_mag->ammo.size());
				}
				this->magazine.ammo[0].get_name() = this->ptr_mag->ammo[0].get_name();
			}
			if (this->magazine.cap == 0) {
				this->del_timer = 5.f;
			}
			else {
				this->del_timer = 20.f;
			}
		}
		bool Set_item_magrelease(const size_t& dnm, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr) {
				this->Set_item_1(magdata, pos_, add_, mat_);
				this->magazine.cap = dnm;
				if (this->ptr_mag != nullptr) {
					if (this->magazine.ammo.size() < this->ptr_mag->ammo.size()) {
						this->magazine.ammo.resize(this->ptr_mag->ammo.size());
					}
					this->magazine.ammo[0].get_name() = this->ptr_mag->ammo[0].get_name();
				}
				if (this->magazine.cap == 0) {
					this->del_timer = 5.f;
				}
				else {
					this->del_timer = 20.f;
				}
				return true;
			}
			return false;
		}
		//item
		void Set_item_med_(const size_t& id, Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->id_t = id;
			this->Set_item(meddata, pos_, add_, mat_);
		}
		bool Set_item_med(Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr) {
				this->Set_item(meddata, pos_, add_, mat_);
				return true;
			}
			return false;
		}
		//
		template<class Y, class D>
		void UpDate(std::vector<Items>& item, std::unique_ptr<Y, D>& MAPPTs) {
			const auto fps_ = GetFPS();
			auto old = this->pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr) {
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add_vec;
				this->add_vec.yadd(M_GR / powf(fps_, 2.f));
				for (auto& p : item) {
					if ((p.ptr_mag != nullptr || p.ptr_med != nullptr) && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.35f) {
							p.add_vec.xadd((p.pos - this->pos).x()*5.f / fps_);
							p.add_vec.zadd((p.pos - this->pos).z()*5.f / fps_);
							this->add_vec.xadd((this->pos - p.pos).x()*5.f / fps_);
							this->add_vec.zadd((this->pos - p.pos).z()*5.f / fps_);
						}
					}
				}
				auto pp = MAPPTs->map_col_line(old - VGet(0, 0.0025f, 0), this->pos - VGet(0, 0.0025f, 0));
				if (pp.HitFlag) {
					this->pos = VECTOR_ref(pp.HitPosition) + VECTOR_ref(pp.Normal)*0.005f;
					this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(pp.Normal)*-1.f);
					this->add_vec.clear();
					//easing_set(&this->add_vec, VGet(0, 0, 0), 0.8f);
				}
				//
			}
			//
			if (this->ptr_mag != nullptr) {
				this->del_timer -= 1.f / GetFPS();
			}
		}
		template<class Y, class D, class Chara>
		void Get_item_2(VECTOR_ref StartPos, VECTOR_ref EndPos, Chara& chara, std::unique_ptr<Y, D>& MAPPTs) {
			bool zz = false;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr) {
				auto p = MAPPTs->map_col_line(StartPos, EndPos);
				if (p.HitFlag) {
					EndPos = p.HitPosition;
				}
				zz = (Segment_Point_MinLength(StartPos.get(), EndPos.get(), this->pos.get()) <= 0.4f);
			}
			//個別
			if (this->ptr_mag != nullptr) {
				chara.get_canget_magitem() |= zz;
				if (zz) {
					chara.get_canget_id() = this->id_t;
					chara.get_canget_mag() = this->ptr_mag->mod.get_name();
					if (chara.getmagazine_push() && this->magazine.cap != 0 && (this->ptr_mag->ammo[0].get_name() == this->magazine.ammo[0].get_name())) {
						chara.sort_f = false;
						chara.gun_stat_now->magazine_plus(&(this->magazine));
						if (chara.gun_stat_now->get_magazine_in().size() == 1) {
							chara.reloadf = true;
						}
						this->Detach_item();
					}
				}
			}
			//
			if (this->ptr_med != nullptr) {
				chara.get_canget_meditem() |= zz;
				if (zz) {
					chara.get_canget_id() = this->id_t;
					chara.get_canget_med() = this->ptr_med->mod.get_name();
					if (chara.getmagazine_push()) {
						chara.HP = std::clamp<int>(chara.HP + this->ptr_med->repair, 0, chara.HP_full);
						this->Detach_item();
					}
				}
			}
		}
		void Draw_item(void) {
			if (this->ptr_mag != nullptr) {
				this->obj.DrawModel();
			}
			if (this->ptr_med != nullptr) {
				this->obj.DrawModel();
			}
		}
		void Detach_item(void) {
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->obj.Dispose();
		}
		bool Detach_mag(void) {
			if (this->ptr_mag != nullptr && this->del_timer <= 0.f) {
				this->Detach_item();
				return true;
			}
			return false;
		}
	};
};
//