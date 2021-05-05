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
		char onhandle[256], offhandle[256];
	};
	class key_pair {
	public:
		int first;
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
	void load_keyg() {
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
	void reSet_isalways() {
		for (auto& i : this->key_use_ID) {
			i.isalways = false;
		}
		for (auto& i : this->mouse_use_ID) {
			i.isalways = false;
		}
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
		if (get_end()) {
			timer = 0.f;
		}
	}
};
//
class Mainclass {
private:
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
	//銃、マガジン共通モデル
	class Models {
	private:
		std::string name;
		std::string path;
		MV1 model;
	public:
		auto& get_name() { return name; }
		auto& get_path() { return path; }
		auto& get_model() { return model; }

		void Ready(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/model.mv1", &this->model, true);
		}

		int mdata = -1;
		template <typename T>
		void Set_(T doing) {
			this->model.SetMatrix(MGetIdent());
			this->mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			doing();
			FileRead_close(mdata);
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
protected:
	class Chara;
	class Items;
	//パーツデータ
	class GUNPARTs {
	private:
		class frame_gun {
		public:
			frames magazine_f;
			frames magazine2_f;
			frames mazzule_f;
			frames site_f;
			frames right_f;

			frames cate_f;

			void ReSet(void) {
				magazine_f.first = INT_MAX;
				magazine2_f.first = INT_MAX;
				mazzule_f.first = INT_MAX;
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
					else if (p == std::string("site")) {
						this->site_f = { i, obj_.frame(i) };//アイアンサイト
					}
					else if (p == std::string("RIGHT")) {
						this->right_f = { i, obj_.frame(i) };//右手
					}
					else if (p == std::string("cate")) {
						this->cate_f = { i, obj_.frame(i) };//排莢
					}
				}
			}

		};
	public:
		//common
		size_t id_t = 0;
		int type = 0;
		Models mod;
		performance per;
		std::vector<std::string> can_attach;
		//base
		frame_gun frame_s;
		std::vector <uint8_t> select;	//セレクター
		std::vector<Ammos> ammo;
		Audios audio;
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//再装填時間
		//mazzule
		size_t mazzule_type = 0;
		//stock
		size_t stock_type = 0;
		//sight
		GraphHandle reticle;
		//magazine
		size_t cap = 1;
		//
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
				this->frame_s.ReSet();
				this->frame_s.get_frame(this->mod.get_model());
				//
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
			this->mat = MATRIX_ref::Scale(scale)*  MATRIX_ref::Axis1(y_vec.cross(z_vec), y_vec, z_vec);
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
	//player
	class Chara {
	private:
		//実際に発射される弾
		class BULLETS {
		private:
		public:
			bool hit_Flag = false;
			float hit_cnt = 0.f;
			float hit_alpha = 0.f;
			int hit_window_x = 0;
			int hit_window_y = 0;
			bool Flag = false;
			bool DrawFlag = false;
			float cnt = 0.f;
			Ammos* spec = nullptr;
			float yadd = 0.f;
			VECTOR_ref pos;
			MATRIX_ref mat;

			VECTOR_ref repos;
			void Set(void) {
				this->Flag = false;
				this->DrawFlag = false;
			}
			void Put(Ammos* spec_t, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) {
				this->hit_Flag = false;
				this->Flag = true;
				this->DrawFlag = true;
				this->cnt = 0.f;
				this->spec = spec_t;
				this->yadd = 0.f;

				this->pos = pos_t;
				this->repos = this->pos;
				this->mat = mat_t;
			}
			template<class Y, class D>
			void UpDate(Chara* c, std::vector<Chara>* chara, std::unique_ptr<Y, D>& MAPPTs, std::vector<Hit>& hit_obj, size_t& hitbuf) {
				if (this->Flag) {
					this->repos = this->pos;
					this->pos += this->mat.zvec() * (this->spec->get_speed() / GetFPS());
					//判定
					{
						auto p = MAPPTs->map_col_line(this->repos, this->pos);
						if (p.HitFlag) {
							this->pos = p.HitPosition;
						}
						//*
						for (auto& tgt : *chara) {
							if (!tgt.get_alive()) {
								continue;
							}
							{
								auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 0);
								if (q.HitFlag) {
									this->pos = q.HitPosition;
									//hit
									c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
									//
									this->hit_Flag = true;
									this->Flag = false;

									auto old = tgt.HP;
									tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 3 / 5, 0, tgt.HP_full);
									tgt.got_damage = old - tgt.HP;
									tgt.got_damage_color = GetColor(255, 255, 255);
									if (float(tgt.HP) / float(tgt.HP_full) <= 0.66) {
										tgt.got_damage_color = GetColor(255, 255, 0);
									}
									if (float(tgt.HP) / float(tgt.HP_full) <= 0.33) {
										tgt.got_damage_color = GetColor(255, 128, 0);
									}
									if ((this->spec->get_damage() * 3 / 5) != tgt.got_damage) {
										tgt.got_damage_color = GetColor(255, 0, 0);
									}

									tgt.got_damage_x = -255 + GetRand(255 * 2);
									tgt.got_damage_f = 1.f;

									if (!tgt.get_alive()) {
										c->kill_f = true;
										c->kill_id = &tgt - &(*chara)[0];
										tgt.death_id = &(*c) - &(*chara)[0];
										if (c->kill_time != 0.f) {
											c->kill_streak++;
											c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
										}
										c->kill_time = 7.f;
										c->score += 50;
										c->kill_cnt++;
										tgt.death_cnt++;
										tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
									}
									else {
										tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
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
									this->hit_Flag = true;
									this->Flag = false;

									auto old = tgt.HP;
									tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 3, 0, tgt.HP_full);
									tgt.got_damage = old - tgt.HP;
									tgt.got_damage_color = GetColor(255, 255, 255);

									if (float(tgt.HP) / float(tgt.HP_full) <= 0.66) {
										tgt.got_damage_color = GetColor(255, 255, 0);
									}
									if (float(tgt.HP) / float(tgt.HP_full) <= 0.33) {
										tgt.got_damage_color = GetColor(255, 128, 0);
									}
									if ((this->spec->get_damage() * 3) != tgt.got_damage) {
										tgt.got_damage_color = GetColor(255, 0, 0);
									}
									tgt.got_damage_x = -255 + GetRand(255 * 2);
									tgt.got_damage_f = 1.f;

									if (!tgt.get_alive()) {
										c->kill_f = true;
										c->kill_id = &tgt - &(*chara)[0];
										tgt.death_id = &(*c) - &(*chara)[0];
										if (c->kill_time != 0.f) {
											c->kill_streak++;
											c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
										}
										c->kill_time = 7.f;
										c->score += 70;
										c->kill_cnt++;
										tgt.death_cnt++;
										tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
									}
									else {
										tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
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
									this->hit_Flag = true;
									this->Flag = false;

									auto old = tgt.HP;
									tgt.HP = std::clamp(tgt.HP - this->spec->get_damage(), 0, tgt.HP_full);
									tgt.got_damage = old - tgt.HP;
									tgt.got_damage_color = GetColor(255, 255, 255);

									if (float(tgt.HP) / float(tgt.HP_full) <= 0.66) {
										tgt.got_damage_color = GetColor(255, 255, 0);
									}
									if (float(tgt.HP) / float(tgt.HP_full) <= 0.33) {
										tgt.got_damage_color = GetColor(255, 128, 0);
									}
									if ((this->spec->get_damage()) != tgt.got_damage) {
										tgt.got_damage_color = GetColor(255, 0, 0);
									}

									tgt.got_damage_x = -255 + GetRand(255 * 2);
									tgt.got_damage_f = 1.f;

									if (!tgt.get_alive()) {
										c->kill_f = true;
										c->kill_id = &tgt - &(*chara)[0];
										tgt.death_id = &(*c) - &(*chara)[0];
										if (c->kill_time != 0.f) {
											c->kill_streak++;
											c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
										}
										c->kill_time = 7.f;
										c->score += 100;
										c->kill_cnt++;
										tgt.death_cnt++;
										tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
									}
									else {
										tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
									}
									break;
								}
							}
						}
						if (p.HitFlag && this->Flag) {
							this->Flag = false;
							c->effcs_gndhit[c->use_effcsgndhit].set(this->pos, p.Normal, 0.025f / 0.1f);
							++c->use_effcsgndhit %= c->effcs_gndhit.size();
							//弾痕のセット
							hit_obj[hitbuf].Put_Hit(this->spec->get_caliber(), this->pos, p.Normal, this->mat.zvec());
							++hitbuf %= hit_obj.size();
						}
						//*/
					}
					//消す(3秒たった、スピードが0以下、貫通が0以下)
					if (this->cnt >= 3.f || this->spec->get_speed() < 0.f || this->spec->get_pene() <= 0.f) {
						this->Flag = false;
					}
					//
				}
			}
			void Set_Draw(void) {
				if (this->hit_Flag) {
					this->hit_Flag = false;
					this->hit_cnt = 0.25f;
					VECTOR_ref p = ConvWorldPosToScreenPos((this->pos).get());
					if (p.z() >= 0.f&&p.z() <= 1.f) {
						this->hit_alpha = 1.f;
						this->hit_window_x = int(p.x());
						this->hit_window_y = int(p.y());
					}
				}
				if (this->hit_cnt != 0.f) {
					easing_set(&this->hit_alpha, 2.f, 0.95f);
				}
				else {
					easing_set(&this->hit_alpha, 0.f, 0.8f);
				}
				this->hit_cnt = std::clamp(this->hit_cnt - 1.f / GetFPS(), 0.f, 1.f);
			}
			void Draw(void) {
				if (this->DrawFlag) {
					DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->get_caliber() - 0.00762f) * 0.1f + 0.00762f), GetColor(255, 255, 172), GetColor(255, 255, 255));
					if (!this->Flag) {
						this->DrawFlag = false;
					}
				}
			}
		};
		//薬莢
		class CARTS {
			MV1 obj;
			bool Flag = false;
			float cnt = -1.f;
			bool se_use = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
			float rate = 0.f;
		public:
			auto& Get_Flag(){ return Flag; }
			auto& Get_pos() { return pos; }

			void Set(void) {
				this->Dispose();
			}
			void Put(MV1&model, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t, const MATRIX_ref& mat_t, const float& rate_t = 1) {
				this->Dispose();
				this->Flag = true;
				this->se_use = false;
				this->cnt = 1.5f;
				this->obj = model.Duplicate();
				this->rate = rate_t;
				this->pos = pos_t;//排莢
				this->add = vec_t * this->rate;//排莢ベクトル
				this->mat = mat_t;
			}
			void UpDate_pos() {
				if (this->Flag) {
					float fps = GetFPS() / this->rate;
					this->cnt -= 1.f / fps;
					this->pos += this->add*(float(200 + GetRand(1000)) / 1000.f);
					this->add.yadd(-9.8f / powf(fps, 2.f));
				}
			}
			void UpDate_fall(const MV1_COLL_RESULT_POLY& mapcol,Chara& mine) {
				if (this->Flag) {
					if (mapcol.HitFlag) {
						VECTOR_ref map_nomal = mapcol.Normal;
						this->pos = VECTOR_ref(mapcol.HitPosition) + VGet(0, 0.008f, 0);
						this->add += map_nomal * (map_nomal.dot(this->add*-1.f)*1.25f);
						easing_set(&this->add, VGet(0, 0, 0), 0.95f);
						if (!this->se_use) {
							mine.audio.case_down.play_3D(mine.pos_tt + mine.pos_gun, 1.f);
							this->se_use = true;
						}
						this->mat *= MATRIX_ref::RotVec2(this->mat.yvec(), map_nomal);
					}
					else {
						this->mat *= MATRIX_ref::RotY(deg2rad(360 * 3 + GetRand(360 * 3*2))* this->rate / GetFPS());
					}
					this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
					if (this->cnt <= 0.f) {
						this->Flag = false;
					}
				}
			}
			void Draw(void) {
				if (this->Flag) {
					this->obj.DrawModel();
				}
			}
			void Dispose(void) {
				this->Flag = false;
				this->obj.Dispose();
			}
		};
		//所持弾数などの武器データ
		class GUN_STATUS {
			size_t ammo_total = 0;				//総所持弾数
			size_t ammo_cnt = 0;				//現在の装弾数カウント
			uint8_t select = 0;					//セレクター
			std::vector<size_t> magazine_in;	//マガジン内データ
		public:
			auto& get_select() { return select; }
			std::vector<size_t>& get_magazine_in() { return magazine_in; }
			auto& get_ammo_cnt() { return ammo_cnt; }
			auto& get_ammo_total() { return ammo_total; }
			auto& magazine_in_flont() { return magazine_in.front(); }


			void Set(void) {
				this->ammo_total = 0;
				this->magazine_in.clear();
			}
			//マガジンを1つ追加(装填無し)
			void magazine_plus(GUNPARTs* magazine) {
				this->ammo_total += magazine->cap;
				this->magazine_in.insert(this->magazine_in.end(), magazine->cap);
			}
			//マガジンを1つ追加(装填あり)
			void magazine_insert(GUNPARTs* magazine) {
				magazine_plus(magazine);
				if (this->not_mags_EMPTY()) {
					this->ammo_cnt = this->magazine_in_flont();//改良
				}
				else {
					this->ammo_cnt = std::clamp<size_t>(this->ammo_total, 0, magazine->cap);//改良
				}
			}
			//マガジンを落とす
			void magazine_release(void) {
				size_t dnm = this->ammo_cnt - 1;
				//弾数
				if (this->not_EMPTY()) {
					this->ammo_cnt = 1;
				}
				else {
					dnm = 0;
				}
				this->ammo_total -= dnm;
				this->magazine_in.erase(this->magazine_in.begin());
			}
			//マガジンを落とす(末尾)
			void magazine_release_end(void) {
				this->magazine_in.pop_back();
			}
			//射撃する
			void magazine_shot(const bool& reloadf) {
				this->ammo_cnt--;
				this->ammo_total--;
				if (!reloadf && this->not_mags_EMPTY() && this->magazine_in_flont() > 0) {
					this->magazine_in_flont()--;
				}
			}
			//スライドを引いて装填
			void magazine_slide(void) {
				if (this->ammo_cnt <= 1) {
					this->ammo_cnt += this->magazine_in_flont();
				}
			}
			//
			auto getminus(void) {
				return (this->not_EMPTY()) ? this->ammo_cnt - 1 : 0;
			}
			//
			bool not_mags_EMPTY(void) { return magazine_in.size() >= 1; }
			//
			bool not_EMPTY(void) { return ammo_cnt > 0; }
			//
		};
		//
		class frame_body {
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

			void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) {
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
		//
		class opes {
		public:
			//
			bool wkey = false;
			bool skey = false;
			bool akey = false;
			bool dkey = false;
			bool qkey = false;
			bool ekey = false;
			//
			bool running = false;			//走るか否か
			bool jamp = false;				//ジャンプ
			bool shoot = false;				//射撃
			bool reload = false;			//リロード開始
			bool have_magazine = false;		//マガジンを持つか
			bool have_item = false;			//アイテム取得
			bool sort_magazine = false;		//マガジンソート
			bool delete_ = false;			//医療品落とす
			bool aim = false;				//エイム
			bool squat = false;				//しゃがむ
			bool select = false;			//セレクター

			void ReSet_(void) {
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
				this->have_item = false;
				this->sort_magazine = false;
				this->delete_ = false;
				this->select = false;
				this->have_magazine = true;
				this->jamp = false;
				this->squat = false;

			}
			void Set(const bool&isVR, key_bind* k_) {
				if (!isVR) {
					if (k_ != nullptr) {
						//記憶
						k_->key_use_ID[9].on_off.first = this->squat;
						//
						this->wkey = k_->key_use_ID[0].get_key(0);
						this->skey = k_->key_use_ID[1].get_key(0);
						this->dkey = k_->key_use_ID[2].get_key(0);
						this->akey = k_->key_use_ID[3].get_key(0);
						this->qkey = k_->key_use_ID[4].get_key(0);
						this->ekey = k_->key_use_ID[5].get_key(0);
						this->reload = k_->key_use_ID[6].get_key(0);
						this->have_item = k_->key_use_ID[7].get_key(2);
						this->delete_ = k_->key_use_ID[8].get_key(2);
						this->squat = k_->key_use_ID[9].get_key(1);

						this->sort_magazine = k_->key_use_ID[12].get_key(2);
						this->running = k_->key_use_ID[13].get_key(0);
						this->jamp = k_->key_use_ID[14].get_key(0);

						this->shoot = k_->mouse_use_ID[0].get_key(3);
						this->select = k_->mouse_use_ID[1].get_key(5);
						this->aim = k_->mouse_use_ID[2].get_key(3);
					}
					this->have_magazine = true;
				}
				else {

				}
			}

			void Set_VR(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
				if (DrawPts->get_hand1_num() != -1) {
					auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hand1_num()];
					if (ptr_.turn && ptr_.now) {
						this->shoot = ((ptr_.on[0] & BUTTON_TRIGGER) != 0);																				//射撃
						this->reload = ((ptr_.on[0] & BUTTON_SIDE) != 0);																					//マグキャッチ
						this->select = ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f&&ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hand2_num()];
					if (ptr_.turn && ptr_.now) {
						this->have_magazine = ((ptr_.on[0] & BUTTON_TRIGGER) != 0);		//マガジン持つ
						this->have_item = (ptr_.on[0] & BUTTON_TOPBUTTON_B) != 0;	//アイテム取得
						this->sort_magazine = false;									//
						this->delete_ = false;									//
						this->running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;		//running
						this->jamp = (ptr_.on[0] & BUTTON_SIDE) != 0;			//jamp
					}
				}
			}
			void rule_(void) {
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
			void UpDate(void) {
			}
		};
	public:
		class g_parts {
			size_t type = 0;
			frames rad_parts;
			VECTOR_ref vec_parts;
			bool attach = false;
			//専用
			frames mazzule_frame;				//マズル
			frames attach_frame;				//根元
			frames source_frame;				//光源
			frames sight_frame;					//サイト
			std::array<frames, 2> LEFT_frame;	//左手座標
			std::array<frames, 2> magazine_f;	//マガジンフレーム
			GraphHandle reticle;				//レティクル
			int LightHandle = -1;				//ライト
		public:
			MV1 obj;
			//MV1 col;

			std::array<MV1, 2> obj_ammo;			//マガジン用弾
			std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム

			std::array<std::array<frames, 2>, EnumAttachPoint::NUM_> rail_f;

			GUNPARTs* thisparts = nullptr;
			g_parts* attach_parts = nullptr;

			auto& get_type() { return type; }
			auto& get_attaching() { return attach; }
			auto& get_attach_frame() { return attach_frame.second; }
			auto& get_sight_frame() { return sight_frame.second; }

			const int get_mazzuletype() {
				if (this->attach) {
					return int(this->thisparts->mazzule_type);
				}
				return -1;
			}
			const int get_stocktype() {
				if (this->attach) {
					return int(this->thisparts->stock_type);
				}
				return -1;
			}
			const VECTOR_ref mazzule_pos() {
				if (this->attach) {
					return this->obj.frame(this->mazzule_frame.first);
				}
				return VGet(0, 0, 0);
			}
			const VECTOR_ref mazzule_none_pos() {
				if (this->attach) {
					return this->obj.frame(this->thisparts->frame_s.mazzule_f.first);
				}
				return VGet(0, 0, 0);
			}
			const VECTOR_ref LEFT_pos() {
				if (this->attach) {
					return this->obj.frame(this->LEFT_frame[0].first);
				}
				return VGet(0, 0, 0);
			}
			const VECTOR_ref RIGHT_pos() {
				if (this->attach) {
					return this->obj.frame(this->thisparts->frame_s.right_f.first);
				}
				return VGet(0, 0, 0);
			}
			const VECTOR_ref magf_pos() {
				if (this->attach) {
					return this->obj.frame(this->thisparts->frame_s.magazine_f.first);
				}
				return VGet(0, 0, 0);
			}

			auto& getmagazine_f(const int& i) { return magazine_f[i]; }

			const VECTOR_ref mag2f_pos() {
				if (this->attach) {
					return this->obj.frame(this->thisparts->frame_s.magazine2_f.first);
				}
				return VGet(0, 0, 0);
			}

			const VECTOR_ref source_frame_pos() {
				if (this->attach) {
					return this->obj.frame(this->source_frame.first);
				}
				return VGet(0, 0, 0);
			}
			g_parts(void) {
				this->LightHandle = -1;
			}
			void Draw(const size_t mag_in = 0) {
				if (this->attach) {
					this->obj.DrawModel();
					if (this->type == EnumGunParts::PARTS_MAGAZINE) {
						if (mag_in >= 1) {
							this->obj_ammo[0].DrawModel();
						}
						if (mag_in >= 2) {
							this->obj_ammo[1].DrawModel();
						}
					}
				}
			}
			void ReSet_frame(void) {
				for (auto& r : this->rail_f) {
					r[0].first = -1;
					r[1].first = -1;
				}
				rad_parts.first = -1;
				attach_frame.first = -1;
			}
			void get_frame_(GUNPARTs& parts, const size_t& type_t) {
				if (this->attach) {
					this->thisparts = &parts;
					this->type = type_t;
					{
						this->ReSet_frame();
						SetUseASyncLoadFlag(FALSE);
						if (this->type == EnumGunParts::PARTS_BASE) {
							this->thisparts->mod.get_model().DuplicateonAnime(&this->obj);
						}
						else {
							this->obj = this->thisparts->mod.get_model().Duplicate();
						}

						if (this->type == EnumGunParts::PARTS_MAGAZINE) {
							this->obj_ammo[0] = this->thisparts->ammo[0].get_model_full().Duplicate();
							this->obj_ammo[1] = this->thisparts->ammo[0].get_model_full().Duplicate();
						}
						/*
						if (this->type == EnumGunParts::PARTS_SIGHT) {
							this->col = this->thisparts->mod.get_model().Duplicate();
						}
						*/

						SetUseASyncLoadFlag(FALSE);
						for (int i = 0; i < int(this->obj.frame_num()); i++) {
							std::string p = this->obj.frame_name(i);
							if (p == std::string("rail")) {
								this->rail_f[EnumAttachPoint::UPER_RAIL][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UPER_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("underrail")) {
								this->rail_f[EnumAttachPoint::UNDER_RAIL][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::UNDER_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("siderail")) {
								this->rail_f[EnumAttachPoint::LEFTSIDE_RAIL][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::LEFTSIDE_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("rightrail")) {
								this->rail_f[EnumAttachPoint::RIGHTSIDE_RAIL][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::RIGHTSIDE_RAIL][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
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
								this->rail_f[EnumAttachPoint::MAZZULE_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::MAZZULE_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("grip")) {
								this->rail_f[EnumAttachPoint::GRIP_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::GRIP_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("rad_parts")) {
								this->rad_parts = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("sight")) {
								this->sight_frame = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("LEFT")) {
								this->LEFT_frame[0] = { int(i),this->obj.frame(i) };
								this->LEFT_frame[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("source")) {
								this->source_frame = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("magazine_fall")) {
								this->rail_f[EnumAttachPoint::MAGAZINE_BASE][0] = { int(i),this->obj.frame(i) };
								this->rail_f[EnumAttachPoint::MAGAZINE_BASE][1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
								this->magazine_f[0] = { int(i),this->obj.frame(i) };
								this->magazine_f[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
							}
							if (p == std::string("ammo1")) {
								this->magazine_ammo_f[0] = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("ammo2")) {
								this->magazine_ammo_f[1] = { int(i),this->obj.frame(i) };
							}
							if (p == std::string("mazzule")) {
								this->mazzule_frame = { int(i),this->obj.frame(i) };
							}
						}
					}
					switch (this->type) {
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
			void Draw_reticle() {
				if (this->attach) {
					if (this->type == EnumGunParts::PARTS_SIGHT) {
						auto ret = this->obj.frame(3);
						auto camp = VECTOR_ref(GetCameraPosition());
						if (this->obj.CollCheck_Line(camp, ret, 0, 1).HitFlag == TRUE) {
							SetUseZBuffer3D(FALSE);
							DrawBillboard3D(ret.get(), 0.5f, 0.5f, 4.f, 0.f, this->reticle.get(), TRUE);
							SetUseZBuffer3D(TRUE);
						}
					}
				}
			}
			void Set_reticle(void) {
				if (this->attach) {
					this->obj.RefreshCollInfo(0, 1);
				}
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, const size_t& type_t) {
				this->attach = true;
				if (this->attach) {
					this->get_frame_(partsptr, type_t);
					per_all += this->thisparts->per;
				}
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, const size_t& type_t, g_parts* parents, size_t side) {
				this->attach = false;
				if (parents != nullptr) {
					if (parents->thisparts != nullptr) {
						for (auto& p : parents->thisparts->can_attach) {
							if (p == partsptr.mod.get_name()) {
								this->attach = (parents->rail_f[side][0].first > 0);
								if (this->attach) {
									this->get_frame_(partsptr, type_t);
									per_all += this->thisparts->per;
								}
								if (this->attach) {
									this->attach_parts = parents;
									this->attach_frame = parents->rail_f[side][0];
									this->vec_parts = parents->rail_f[side][1].second.Norm();
								}
								break;
							}
						}
					}
				}
			}
			void detach_(performance& per_all) {
				if (this->attach) {
					this->attach = false;
					this->ReSet_frame();
					per_all -= this->thisparts->per;
					this->thisparts = nullptr;
					this->attach_parts = nullptr;
					this->type = EnumGunParts::PARTS_NONE;
					MV1TerminateCollInfo(this->obj.get(), 0, 1);
					this->obj.Dispose();
					this->reticle.Dispose();
				}
			}
			void Setpos_parts(const MATRIX_ref& mat_t, const size_t& sel2) {
				if (this->attach) {
					auto fpos_t = MATRIX_ref(mat_t)*MATRIX_ref::Mtrans(this->attach_parts->obj.frame(this->attach_frame.first));
					if (sel2 == 1)
					{
						fpos_t = fpos_t * MATRIX_ref::Mtrans(mat_t.pos()*-1.f);
					}
					this->obj.SetMatrix(MATRIX_ref::RotVec2(this->rad_parts.second.Norm(), this->vec_parts)*fpos_t);//方向指定
					
					if (this->type == EnumGunParts::PARTS_MAGAZINE) {
						this->obj_ammo[0].SetMatrix(this->obj.GetMatrix() * MATRIX_ref::Mtrans(this->obj.frame(this->magazine_ammo_f[0].first) - this->obj.GetMatrix().pos()));
						this->obj_ammo[1].SetMatrix(this->obj.GetMatrix() * MATRIX_ref::Mtrans(this->obj.frame(this->magazine_ammo_f[1].first) - this->obj.GetMatrix().pos()));
					}
				}
			}
			void Set_LightHandle(const VECTOR_ref& Vec) {
				if (this->attach) {
					VECTOR_ref StartPos = this->source_frame_pos();
					if (this->LightHandle == -1) {
						this->LightHandle = CreateSpotLightHandle(StartPos.get(), Vec.get(), DX_PI_F / 16.0f, DX_PI_F / 24.0f, 25.0f, 0.1f, 0.35f, 0.0f);
					}
					SetLightPositionHandle(this->LightHandle, StartPos.get());
					SetLightDirectionHandle(this->LightHandle, Vec.get());
				}
				else {
					if (this->LightHandle != -1) {
						DeleteLightHandle(this->LightHandle);
						this->LightHandle = -1;
					}
				}
			}
		};
	private:
		MV1 obj;
		//プレイヤー座標系
		VECTOR_ref pos_tt;				//位置
		MATRIX_ref mat;				//位置
		VECTOR_ref add_vec;			//移動ベクトル
	private:
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<BULLETS, 64> bullet;
		std::array<CARTS, 64> cart;
		size_t use_bullet = 0;
		/*所持弾数などの武器データ*/
		std::vector<GUN_STATUS> gun_stat;
		/*音声*/
		Audios audio;
		/*anime*/
		//脚
		MV1::ani* gunanime_shot = nullptr;
		MV1::ani* gunanime_first = nullptr;
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
		bool LEFT_hand = false;//左手を添えているか
		//
		MV1 col;
		//プレイヤー座標系
		float speed = 0.f;			//
		MATRIX_ref mat_body;		//位置
		VECTOR_ref spawn_pos;		//spawn
		MATRIX_ref spawn_mat;		//spawn
		VECTOR_ref add_vec_buf;		//移動ベクトルバッファ
		float xrad_p = 0.f;			//マウスエイム用変数確保
		bool sort_ing = false;		//走るか否か
		VECTOR_ref gunpos;			//マウスエイム用銃座標
		bool gunf = false;			//射撃フラグ
		opes key_;					//入力関連
		VECTOR_ref add_vec_real;	//実際の移動ベクトル
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
		VECTOR_ref blur_vec, blur_vec_res;
		VECTOR_ref recoil_vec, recoil_vec_res;
		//頭部座標系
		VECTOR_ref pos_HMD, rec_HMD;
		VECTOR_ref pos_HMD_old;
		float add_ypos = 0.f;		//垂直加速度
		float body_xrad = 0.f;		//胴体角度
		float body_yrad = 0.f;		//胴体角度
		float body_zrad = 0.f;		//胴体角度
		float body_ztrun = 0.f;		//胴体角度
		//フレーム
		float head_hight = 0.f;
		float head_hight2 = 0.f;
		//右手座標系
		VECTOR_ref pos_RIGHTHAND;
		MATRIX_ref mat_RIGHTHAND;
		//右手座標系
		MATRIX_ref mat_RIGHTREG_rep;
		VECTOR_ref pos_RIGHTREG;
		MATRIX_ref mat_RIGHTREG;
		//左手座標系
		VECTOR_ref pos_LEFTHAND;
		MATRIX_ref mat_LEFTHAND;
		//左手座標系
		MATRIX_ref mat_LEFTREG_rep;
		VECTOR_ref pos_LEFTREG;
		MATRIX_ref mat_LEFTREG;
		//
		MATRIX_ref mat_WAIST_rep;
		//
		int score = 0;
		int kill_cnt = 0;
		int death_cnt = 0;
		//
		bool Start_a = true;
		bool Start_b = true;
		bool Start_c = true;
		bool hit_f = false;
		float hit_time = 0.f;
		size_t death_id = 0;
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
		//
		performance per_all;
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
	public:
		/*所持弾数などの武器データ*/
		GUN_STATUS* gun_stat_now;
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
		VECTOR_ref pos_WAIST_rep;
		VECTOR_ref pos_WAIST;
		MATRIX_ref mat_WAIST;
		//
		frame_body frame_s;
		frame_body colframe_;
		//
		size_t canget_id = 0;
		//
		unsigned int got_damage_color = 0;
		int got_damage_x = 0;
		float got_damage_f = 0.f;
		int got_damage = 0;
		float HP_r = 100;
		int HP = 100;
		int HP_full = 100;
		//
		bool kill_f = false;
		size_t kill_id = 0;
		float kill_time = 0.f;
		int kill_streak = 0;
		//
		bool sort_f = false;
		bool reloadf = false;
		//
		float rad_gun = 0.f;
		//
	public:
		//
	private:
	public:
	private:
		void delete_child(g_parts* parent) {
			if (this->mazzule.attach_parts == parent) {
				this->mazzule.detach_(this->per_all);
			}
			if (this->grip.attach_parts == parent) {
				this->grip.detach_(this->per_all);
			}
			if (this->uperhandguard.attach_parts == parent) {
				this->uperhandguard.detach_(this->per_all);
			}
			if (this->underhandguard.attach_parts == parent) {
				this->underhandguard.detach_(this->per_all);
			}
			//
			for (auto& m : this->mount_) {
				if (m.attach_parts == parent) {
					m.detach_(this->per_all);
				}
			}
			Check_empty_mount_slot();
			//
			if (this->dustcover.attach_parts == parent) {
				this->dustcover.detach_(this->per_all);
			}
			if (this->stock.attach_parts == parent) {
				this->stock.detach_(this->per_all);
			}
			if (this->foregrip.attach_parts == parent) {
				this->foregrip.detach_(this->per_all);
			}
			if (this->laser.attach_parts == parent) {
				this->laser.detach_(this->per_all);
			}
			if (this->light.attach_parts == parent) {
				this->light.detach_(this->per_all);
			}
			//
			for (auto& s : this->sight_) {
				if (s.attach_parts == parent) {
					s.detach_(this->per_all);
				}
			}
			Check_empty_sight_slot();
		}
		template<class Y, class D>
		void ReSet_waypoint(std::unique_ptr<Y, D>& MAPPTs) {
			int now = -1;
			auto poss = this->get_pos();
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
			auto add_vec_t = (VECTOR_ref(this->base.mag2f_pos()) - this->base.magf_pos()).Norm()*-1.f / fps_;
			for (auto& g : item) {
				if (g.Set_item_2_(dnm, this->magazine.thisparts, this->pos_mag, add_vec_t, this->mat_mag)) {
					tt = true;
					break;
				}
			}
			if (!tt) {
				item.resize(item.size() + 1);
				item.back().Set_item_3_(dnm, item.size() - 1, this->magazine.thisparts, this->pos_mag, add_vec_t, this->mat_mag);
			}
		}
		void magrelease_t(std::vector<Items>& item, const size_t& test_) {
			const auto fps_ = GetFPS();
			if (this->gun_stat_now->not_mags_EMPTY()) {
				//音
				this->audio.magazine_down.play_3D(this->pos_gun, 15.f);
				//弾数
				auto dnm = this->gun_stat_now->getminus();
				if (test_ == 0) {
					while (true) {
						this->gun_stat_now->magazine_release();
						magrelease_tp(item, dnm);
						//
						if (!this->gun_stat_now->not_mags_EMPTY()) {
							break;
						}
						dnm = this->gun_stat_now->magazine_in_flont();
					}
					return;
				}
				if (test_ == 1) {
					this->gun_stat_now->magazine_release();
					magrelease_tp(item, dnm);
					this->reload_cnt = 0.f;
					return;
				}

				if (test_ == 2) {
					if (this->gun_stat_now->get_magazine_in().back() == 0) {
						this->gun_stat_now->magazine_release_end();
						magrelease_tp(item, 0);
						this->reload_cnt = 0.f;
						return;
					}
				}
			}
		}
		void Set_gun2(void) {
			auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* this->obj.GetFrameLocalWorldMatrix(this->frame_s.RIGHThand2_f.first);
			this->pos_gun = this->obj.frame(this->frame_s.RIGHThand_f.first);
			//this->Set_gun();
			this->base.obj.SetMatrix(mat_T);
			this->base.obj.SetMatrix(mat_T*MATRIX_ref::Mtrans(this->pos_gun - this->base.RIGHT_pos()));
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
			this->mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (this->obj.GetFrameLocalWorldMatrix(this->frame_s.LEFThand2_f.first)*MATRIX_ref::Mtrans(this->obj.frame(this->frame_s.LEFThand2_f.first)).Inverse());
			this->pos_LEFTHAND = this->obj.frame(this->frame_s.LEFThand_f.first) + this->mat_LEFTHAND.yvec()*0.1f;
		}
		void calc_hmd() {
			this->pos_HMD = (this->obj.frame(this->frame_s.RIGHTeye_f.first) + (this->obj.frame(this->frame_s.LEFTeye_f.first) - this->obj.frame(this->frame_s.RIGHTeye_f.first))*0.5f) - this->pos_tt;
		}
		void calc_gun(void) {
			if (this->get_alive()) {
				if (this->running() || (this->reloadf && this->gun_stat_now->not_mags_EMPTY()) || this->anime_arm_check->per == 1.f) {
					Set_gun2();
				}
				else {
					Set_gun();
				}
			}
		}
		//
	public:
		//
		auto& get_speed() { return speed; }
		bool get_alive() { return this->HP != 0; }
		auto& get_kill_cnt() { return kill_cnt; }
		auto& get_death_cnt() { return death_cnt; }
		auto& get_score() { return score; }
		auto& get_audio() { return audio; }
		auto& get_per_all() { return per_all; }
		auto get_head_frame() { return this->obj.frame(this->frame_s.head_f.first); }
		auto& get_pos_tt() { return pos_tt; }
		auto get_pos() { return this->pos_tt + this->pos_HMD - this->rec_HMD; }
		auto& get_key_() { return key_; }
		bool getmagazine_push() { return this->key_.have_item; }
		bool squat_on() { return this->key_.squat; }
		bool ads_on() { return this->key_.aim; }
		bool running() { return this->key_.running; }
		bool get_wkey() { return this->key_.wkey; }
		bool get_akey() { return this->key_.akey; }
		bool get_skey() { return this->key_.skey; }
		bool get_dkey() { return this->key_.dkey; }
		bool get_shoot() { return this->key_.shoot; }
		auto& get_gunf() { return gunf; }
		auto& get_add_vec_real() { return add_vec_real; }
		VECTOR_ref& get_pos_RIGHTHAND() { return pos_RIGHTHAND; }
		MATRIX_ref& get_mat_RIGHTHAND() { return mat_RIGHTHAND; }
		VECTOR_ref& get_pos_LEFTHAND() { return pos_LEFTHAND; }
		MATRIX_ref& get_mat_LEFTHAND() { return mat_LEFTHAND; }
		bool& get_canget_gunitem() { return canget_gunitem; }
		bool& get_canget_magitem() { return canget_magitem; }
		bool& get_canget_meditem() { return canget_meditem; }
		std::string & get_canget_gun() { return canget_gun; }
		std::string & get_canget_mag() { return canget_mag; }
		std::string & get_canget_med() { return canget_med; }
		auto& get_gunanime_trigger() { return gunanime_trigger; }
	private:
		//
		template<class Y2, class D2>
		void calc_shot(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::vector<Chara>&chara, std::vector<Hit>& hit_obj, size_t& hit_buf, std::vector <Meds>& meds_data, std::vector<Items>& item) {
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
			//手振れ
			easing_set(&this->blur_vec, this->blur_vec_res, 0.975f);
			easing_set(&this->blur_vec_res, VGet(0, 0, 1.f), 0.95f);
			//複座
			easing_set(&this->recoil_vec, this->recoil_vec_res, 0.6f);
			easing_set(&this->recoil_vec_res, VGet(0, 0, 1.f), 0.95f);
			//リコイルアニメーション
			if (this->gunf) {
				if (this->gun_stat_now->not_EMPTY()) {
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
				if (this->gun_stat_now->not_mags_EMPTY()) {
					this->reloadf = true;
				}
				magrelease_t(item, 1);
			}
			if (!this->reloadf && this->gunanime_magcatch->per >= 0.5f && this->gun_stat_now->not_mags_EMPTY()) {
				this->reloadf = true;
				//音
				this->audio.magazine_down.play_3D(this->pos_gun, 15.f);
				//弾数
				auto dnm = this->gun_stat_now->getminus();
				this->gun_stat_now->magazine_release();
				this->reload_cnt = 0.f;
				bool tt = false;
				auto add_vec_t = (VECTOR_ref(this->base.mag2f_pos()) - this->base.magf_pos()).Norm()*-1.f / fps_;
				for (auto& g : item) {
					if (g.Set_item_2_(dnm, this->magazine.thisparts, this->pos_mag, add_vec_t, this->mat_mag)) {
						tt = true;
						break;
					}
				}
				if (!tt) {
					item.resize(item.size() + 1);
					item.back().Set_item_3_(dnm, item.size() - 1, this->magazine.thisparts, this->pos_mag, add_vec_t, this->mat_mag);
				}
			}
			//medkit生成
			if (this->key_.delete_) {
				bool tt = false;
				auto add_vec_t = (this->base.obj.GetMatrix().zvec()).Norm()*-5.f / fps_;
				for (auto& g : item) {
					if (g.Set_item_2(&meds_data[0], this->base.mazzule_none_pos(), add_vec_t, this->mat_mag)) {
						tt = true;
						break;
					}
				}
				if (!tt) {
					item.resize(item.size() + 1);
					item.back().Set_item_3(item.size() - 1, &meds_data[0], this->base.mazzule_none_pos(), add_vec_t, this->mat_mag);
				}
			}
			//マガジン整頓
			if (!this->sort_ing && this->key_.sort_magazine && this->gun_stat_now->get_magazine_in().size() >= 2) {
				this->sort_ing = true;
				if (!this->sort_f) {
					this->audio.sort_magazine.play_3D(this->pos_gun, 15.f);
					std::sort(this->gun_stat_now->get_magazine_in().begin() + 1, this->gun_stat_now->get_magazine_in().end(), [](size_t a, size_t b) { return a > b; });
					this->sort_f = true;
				}
				else {
					this->audio.load_.play_3D(this->pos_gun, 15.f);
					size_t siz = 0;
					for (auto& m : this->gun_stat_now->get_magazine_in()) {
						if ((&m != &this->gun_stat_now->get_magazine_in()[0]) && (m != this->magazine.thisparts->cap)) {
							siz = &m - &this->gun_stat_now->get_magazine_in()[0];
							break;
						}
					}
					auto be_ = std::clamp<size_t>(this->gun_stat_now->get_magazine_in().back(), 0, this->magazine.thisparts->cap - this->gun_stat_now->get_magazine_in()[siz]);
					this->gun_stat_now->get_magazine_in().back() -= be_;
					this->gun_stat_now->get_magazine_in()[siz] += be_;

					magrelease_t(item, 2);
				}
			}
			//マガジン挿入
			if (this->reloadf && this->gun_stat_now->not_mags_EMPTY()) {
				if (DrawPts->use_vr && this->reload_cnt < this->base.thisparts->reload_time) {
					this->key_.have_magazine = false;
				}
				if (this->key_.have_magazine) {
					auto mag0 = this->magazine.getmagazine_f(0).first;
					auto mag1 = this->magazine.getmagazine_f(1).first;
					if (
						(DrawPts->use_vr) ?
						((this->magazine.obj.frame(mag1) - this->base.magf_pos()).size() <= 0.1f) :
						(this->reload_cnt > this->base.thisparts->reload_time)
						) {
						this->gunanime_shot->per = 1.f;
						this->gunanime_shot_last->reset();
						if (!this->gun_stat_now->not_EMPTY()) {
							this->gunanime_first->reset();
							this->gunanime_first->per = 1.f;
						}
						this->audio.magazine_Set.play_3D(this->pos_gun, 15.f);
						this->gun_stat_now->magazine_slide();//チャンバーに装填
						this->reloadf = false;
					}
					this->pos_mag = this->pos_LEFTHAND;
					if (DrawPts->use_vr) {
						this->mat_mag =
							this->magazine.obj.GetFrameLocalMatrix(mag1) *
							this->magazine.obj.GetFrameLocalMatrix(mag0) *
							(this->mat_LEFTHAND* MATRIX_ref::RotVec2(this->mat_LEFTHAND.yvec(), VECTOR_ref(this->base.mag2f_pos()) - this->pos_LEFTHAND));
					}
					else {
						this->mat_mag = this->mat_LEFTHAND;
					}
				}
				this->reload_cnt += 1.f / fps_;//挿入までのカウント
			}
			else {
				this->key_.have_magazine = false;
				this->pos_mag = this->base.mag2f_pos();
				this->mat_mag = this->mat_gun;
				//todo
				this->mat_mag = this->base.obj.GetMatrix()*MATRIX_ref::Mtrans(this->base.obj.GetMatrix().pos()).Inverse();
			}
			//セレクター
			Set_select();
			if (this->key_.select) {
				++this->gun_stat_now->get_select() %= this->base.thisparts->select.size();
			}
			//射撃
			if (!this->gunf && this->gun_stat_now->not_EMPTY()) {
				if (this->base.thisparts->Select_Chose(2) == int(this->gun_stat_now->get_select())) {
					this->trigger.second = 0;
				}
			}
			{
				//
				float xup = 6.f;
				float xdn = -6.f;
				float yup = 6.f;
				float ydn = -6.f;

				switch (this->stock.get_stocktype()) {
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

				easing_set(&this->blur_vec_res,
					MATRIX_ref::Vtrans(this->blur_vec_res,
						MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn*100.f) + GetRand((int32_t)((xup - xdn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f))))*
						MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn*100.f) + GetRand((int32_t)((yup - ydn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f)))))
					, 0.8f);
			}
			this->trigger.get_in(this->gunanime_trigger->per >= 0.5f);
			if (this->trigger.push()) {
				if (this->mazzule.get_mazzuletype() == 1) {
					this->audio.trigger.vol(164);
				}
				else {
					this->audio.trigger.vol(255);
				}
				this->audio.trigger.play_3D(this->pos_gun, 5.f);
				if (!this->gunf && this->gun_stat_now->not_EMPTY() && this->gunanime_first->per == 0.f) {
					this->gunf = true;
					//弾数管理
					this->gun_stat_now->magazine_shot(this->reloadf);
					//持ち手を持つとココが相殺される
					{
						float xup = this->base.thisparts->recoil_xup*this->per_all.recoil / 100.f;
						float xdn = this->base.thisparts->recoil_xdn*this->per_all.recoil / 100.f;
						float yup = this->base.thisparts->recoil_yup*this->per_all.recoil / 100.f;
						float ydn = this->base.thisparts->recoil_ydn*this->per_all.recoil / 100.f;
						this->recoil_vec_res = MATRIX_ref::Vtrans(this->recoil_vec_res,
							MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn*100.f) + GetRand((int32_t)((xup - xdn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f))))*
							MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn*100.f) + GetRand((int32_t)((yup - ydn)*100.f))) / (100.f*(this->LEFT_hand ? 3.f : 1.f)))));
					}
					//排莢、弾
					calc_cart(1.f);
					//エフェクト
					calc_shot_effect();
					//サウンド
					{
						if (this->mazzule.get_mazzuletype() == 1) {
							this->audio.shot.vol(192);
						}
						else {
							this->audio.shot.vol(255);
						}
						this->audio.shot.play_3D(this->pos_gun, 100.f);
					}
				}
			}
			this->Set_mag();
			this->base.obj.work_anime();
			//弾の処理
			for (auto& a : this->bullet) {
				a.UpDate(this, &chara, MAPPTs, hit_obj, hit_buf);
			}
			//薬莢の処理
			update_cart(MAPPTs);
			//エフェクトの処理
			update_effect(DrawPts);
		}
		//
		void Check_empty_mount_slot(void) {
			while (true) {
				int t = -1;
				for (auto& m : this->mount_) {
					if (!m.get_attaching()) {
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
		void Check_empty_sight_slot(void) {
			while (true) {
				int t = -1;
				for (auto& s : this->sight_) {
					if (!s.get_attaching()) {
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
		void move_righthand(const bool& isusewaist, const MATRIX_ref& m_inv, bool ptt) {
			//基準
			this->pos_RIGHTHAND = this->base.RIGHT_pos();
			if (!ptt) {
				this->mat_RIGHTHAND = this->mat_gun;
			}
			VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((this->pos_RIGHTHAND - this->obj.frame(this->frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
			VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(-1.f*sin(deg2rad(30)), -1.f*cos(deg2rad(30)), vec_a1.y() / vec_a1.z())).Norm();//x=0とする
			float cos_t = getcos_tri((this->obj.frame(this->frame_s.RIGHThand_f.first) - this->obj.frame(this->frame_s.RIGHTarm2_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTarm2_f.first) - this->obj.frame(this->frame_s.RIGHTarm1_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTarm1_f.first) - this->pos_RIGHTHAND).size());
			VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			if ((ptt && !isusewaist) || !ptt) {
				this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, (this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHTarm2_f.first) - this->obj.frame(this->frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
			if ((ptt && !isusewaist) || !ptt) {
				this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*(this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
			}
			//下腕
			this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHThand_f.first) - this->obj.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->pos_RIGHTHAND - this->obj.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
			this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
			//手
			this->obj.SetFrameLocalMatrix(this->frame_s.RIGHThand_f.first,
				MATRIX_ref::RotZ(deg2rad(30))*
				MATRIX_ref::RotX(deg2rad(80))*
				this->mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHThand_f.second));
		}
		void move_lefthand(const bool& isusewaist, const MATRIX_ref& m_inv, bool ptt) {
			//基準
			VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((this->pos_LEFTHAND - this->obj.frame(this->frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
			VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
			float cos_t = getcos_tri((this->obj.frame(this->frame_s.LEFThand_f.first) - this->obj.frame(this->frame_s.LEFTarm2_f.first)).size(), (this->obj.frame(this->frame_s.LEFTarm2_f.first) - this->obj.frame(this->frame_s.LEFTarm1_f.first)).size(), (this->obj.frame(this->frame_s.LEFTarm1_f.first) - this->pos_LEFTHAND).size());
			VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			if ((ptt && !isusewaist) || !ptt) {
				this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, (this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFTarm2_f.first) - this->obj.frame(this->frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
			if ((ptt && !isusewaist) || !ptt) {
				this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			if (ptt && isusewaist) {
				this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*(this->mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
			}
			//下腕
			this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFThand_f.first) - this->obj.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->pos_LEFTHAND - this->obj.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
			this->obj.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
			//手
			this->obj.SetFrameLocalMatrix(this->frame_s.LEFThand_f.first, MATRIX_ref::RotZ(deg2rad(-30))* MATRIX_ref::RotX(deg2rad(80))* this->mat_LEFTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFThand_f.second));
		}
		void search_base(VECTOR_ref& pv, g_parts* ptr) {
			if (ptr->attach_parts != &this->base && ptr->attach_parts != nullptr) {
				pv += ptr->attach_parts->get_attach_frame();
				search_base(pv, ptr->attach_parts);
			}
		}
		//
		template<class Y2, class D2>
		void wall_col(std::unique_ptr<Y2, D2>& MAPPTs, Chara& mine, const bool& use_vr) {
			//VR用
			if (use_vr) {
				VECTOR_ref pos_t2 = this->pos_tt + (VECTOR_ref(VGet(this->pos_HMD_old.x(), 0.f, this->pos_HMD_old.z())) - this->rec_HMD);
				VECTOR_ref pos_t = this->pos_tt + (VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				this->pos_tt = pos_t - (VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD);
			}
			//共通
			/*
			{
				VECTOR_ref pos_t2 = this->pos_tt + (VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD;
				VECTOR_ref pos_t = pos_t2 + (this->add_vec / 2);
				MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
				this->pos_tt = pos_t  - (VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) + this->rec_HMD;
			}
			//*/
			{
				VECTOR_ref pos_t2 = (this->pos_tt - this->rec_HMD);
				//VECTOR_ref pos_t = pos_t2 + (this->add_vec / 2);
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
							this->ReSet_waypoint(MAPPTs);
						}
					}
				}
				//反映
				this->pos_tt = pos_t + this->rec_HMD;
				this->add_vec_real = pos_t - pos_t2;
			}
		}
		//
		void operation_2_1(const bool& cannotmove, int32_t x_m, int32_t y_m) {
			if (cannotmove || !this->get_alive()) {
				this->key_.ReSet_();
			}
			this->key_.rule_();

			if (this->get_alive()) {
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


			this->speed = (this->running() ? 6.f : ((this->ads_on() ? 2.f : 4.f)*(this->squat_on() ? 0.4f : 1.f))) / GetFPS();
			VECTOR_ref zv_t = this->mat.zvec();
			zv_t.y(0.f);
			zv_t = zv_t.Norm();
			VECTOR_ref xv_t = this->mat.xvec();
			xv_t.y(0.f);
			xv_t = xv_t.Norm();
			if (this->running()) {
				xv_t = xv_t.Norm()*0.5f;
			}
			easing_set(&this->add_vec_buf,
				VECTOR_ref(VGet(0, 0, 0))
				+ (this->key_.wkey ? (zv_t*-this->speed) : VGet(0, 0, 0))
				+ (this->key_.skey ? (zv_t*this->speed) : VGet(0, 0, 0))
				+ (this->key_.akey ? (xv_t*this->speed) : VGet(0, 0, 0))
				+ (this->key_.dkey ? (xv_t*-this->speed) : VGet(0, 0, 0))
				, 0.95f);
			this->mat_body = (this->running()) ? MATRIX_ref::Axis1(this->mat.yvec().cross(this->add_vec_buf.Norm()*-1.f), this->mat.yvec(), this->add_vec_buf.Norm()*-1.f) : this->mat;
		}
		void operation_VR(const bool& cannotmove, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			//操作
			{
				if (this->get_alive()) {
					if (DrawPts->get_hand2_num() != -1) {
						auto& ptr_ = (*DrawPts->get_device())[DrawPts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
							//移動
							if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
								this->speed = (this->running() ? 8.f : 6.f) / GetFPS();

								if (DrawPts->tracker_num.size() > 0) {
									auto p = this->obj.GetFrameLocalWorldMatrix(this->frame_s.bodyb_f.first);
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
				else {
					if (cannotmove || !this->get_alive()) {
						this->key_.ReSet_();
						//移動
						easing_set(&this->add_vec_buf, VGet(0, 0, 0), 0.95f);
					}
				}
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
		void operation_NOMAL(const bool& cannotmove, const float& fov_per) {
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->get_alive()) {
				GetMousePoint(&x_m, &y_m);
				x_m -= deskx / 2;
				y_m -= desky / 2;
				SetMousePoint(deskx / 2, desky / 2);
				SetMouseDispFlag(FALSE);
			}
			else {
				this->key_.ReSet_();
			}
			//
			x_m = int(float(std::clamp(x_m, -40, 40))*fov_per);
			y_m = int(float(std::clamp(y_m, -40, 40))*fov_per);

			operation_2_1(cannotmove, x_m, y_m);
		}
		template<class Y3, class D3>
		void operation_NPC(std::unique_ptr<Y3, D3>& MAPPTs, std::vector<Chara>&chara, const bool& cannotmove) {
			const auto fps_ = GetFPS();
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->get_alive()) {
				bool pp = true;
				bool is_player = false;
				//操作
				this->key_.aim = false;
				this->key_.reload = false;
				this->key_.have_item = false;
				this->key_.sort_magazine = false;
				this->key_.delete_ = false;
				this->key_.select = false;
				this->key_.have_magazine = true;
				this->key_.shoot = false;
				this->key_.jamp = false;
				//AI
				VECTOR_ref vec_2 = chara[0].obj.frame(chara[0].frame_s.bodyb_f.first) - this->base.obj.GetMatrix().pos();
				if (this->ai_time_shoot < 0.f) {
					vec_2 = chara[0].obj.frame(chara[0].frame_s.head_f.first) - this->base.obj.GetMatrix().pos();
				}
				for (auto& tgt : chara) {
					if (this == &tgt || !tgt.get_alive()) {
						continue;
					}
					auto tmp = tgt.obj.frame(tgt.frame_s.bodyb_f.first) - this->base.obj.GetMatrix().pos();
					if (this->ai_time_shoot < 0.f) {
						tmp = tgt.obj.frame(tgt.frame_s.head_f.first) - this->base.obj.GetMatrix().pos();
					}

					bool tt = true;
					{
						VECTOR_ref StartPos = this->base.obj.GetMatrix().pos();
						VECTOR_ref EndPos = tgt.obj.frame(tgt.frame_s.bodyb_f.first);
						if (this->ai_time_shoot < 0.f) {
							EndPos = tgt.obj.frame(tgt.frame_s.head_f.first);
						}
						if (MAPPTs->map_col_line(StartPos, EndPos).HitFlag) {
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
					VECTOR_ref vec_x = this->obj.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).xvec();
					VECTOR_ref vec_y = this->obj.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).yvec();
					VECTOR_ref vec_z = this->obj.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).zvec()*-1.f;
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
						auto poss = this->obj.GetMatrix().pos();
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
						this->key_.squat = false;
						this->key_.qkey = false;
						this->key_.ekey = false;

						this->ai_reload = false;
						this->ai_time_shoot = 0.f;

						vec_2 = MAPPTs->get_waypoint()[this->wayp_pre[0]] - this->obj.GetMatrix().pos();
						x_m = -int(vec_x.dot(vec_2) * 120);
						y_m = -int(vec_y.dot(vec_2) * 120);

						//到達時に判断
						if (vec_2.size() <= 0.5f) {
							int now = -1;
							auto poss = this->obj.GetMatrix().pos();
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

						auto poss = this->obj.GetMatrix().pos();
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

						int range = int(1000.f + 1000.f*vec_2.size() / 20.f);
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
							this->key_.squat = false;
							if (is_player) {
								this->key_.shoot = GetRand(100) <= 20;
							}
							else {
								this->key_.shoot = GetRand(100) <= 50;
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
						if (!this->gun_stat_now->not_EMPTY() && !this->reloadf) {
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
						this->key_.squat = false;
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
						auto poss = this->obj.GetMatrix().pos();
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
			x_m = std::clamp(x_m, -40, 40);
			y_m = std::clamp(y_m, -40, 40);
			operation_2_1(cannotmove, x_m, y_m);
		}
		void operation_2(void) {
			//ジャンプ
			{
				if (this->add_ypos == 0.f) {
					if (this->key_.jamp && this->get_alive()) {
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
				easing_set(&this->gunanime_trigger->per, float(this->key_.shoot && !this->running()), 0.5f);
				//マグキャッチ(Rキー)
				easing_set(&this->gunanime_magcatch->per, float(this->key_.reload), 0.5f);
				//その他
				this->key_.UpDate();
			}
		}
		//
		//template<class Y2, class D2>
		void move_VR(/*std::unique_ptr<Y2, D2>& MAPPTs, */std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			if (this->get_alive()) {
				VECTOR_ref v_ = this->mat.zvec();
				if (DrawPts->tracker_num.size() > 0) {
					v_ = this->mat_WAIST.zvec();
				}
				float x_1 = -sinf(this->body_yrad);
				float y_1 = cosf(this->body_yrad);
				float x_2 = v_.x();
				float y_2 = -v_.z();
				this->body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / GetFPS() / 10.f;
			}
			//身体,頭部,腰
			MATRIX_ref m_inv = MATRIX_ref::RotY((DrawPts->tracker_num.size() > 0) ? DX_PI_F : 0 + this->body_yrad);
			{
				if (this->get_alive()) {
					this->obj.SetMatrix(m_inv);
					if (DrawPts->tracker_num.size() > 0) {
						//腰
						this->obj.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, (this->mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
						//頭部
						this->obj.SetFrameLocalMatrix(this->frame_s.head_f.first, (MATRIX_ref::Axis1(this->mat.xvec()*-1.f, this->mat.yvec(), this->mat.zvec()*-1.f) *m_inv.Inverse()*(this->mat_WAIST*m_inv.Inverse()).Inverse())
							*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
					}
					else {
						//頭部
						this->obj.SetFrameLocalMatrix(this->frame_s.head_f.first, (MATRIX_ref::Axis1(this->mat.xvec()*-1.f, this->mat.yvec(), this->mat.zvec()*-1.f) *m_inv.Inverse())
							*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
					}
					this->obj.SetMatrix(m_inv *MATRIX_ref::Mtrans(this->get_pos() - (this->obj.frame(this->frame_s.RIGHTeye_f.first) + (this->obj.frame(this->frame_s.LEFTeye_f.first) - this->obj.frame(this->frame_s.RIGHTeye_f.first))*0.5f)));
				}
				else {
					m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
					this->obj.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->pos_tt + VECTOR_ref(VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z())) - this->rec_HMD));
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
					this->pos_LEFTREG = this->pos_LEFTREG + (this->pos_tt - this->rec_HMD);
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_LEFTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->obj.frame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_LEFTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->obj.frame(this->frame_s.LEFTreg_f.first) - this->obj.frame(this->frame_s.LEFTfoot2_f.first)).size(), (this->obj.frame(this->frame_s.LEFTfoot2_f.first) - this->obj.frame(this->frame_s.LEFTfoot1_f.first)).size(), (this->obj.frame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFTfoot2_f.first) - this->obj.frame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

						//下腕
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFTreg_f.first) - this->obj.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->obj.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						//手
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
					}

					{
						/*
						auto pp = MAPPTs->map_col_line(this->obj.frame(this->frame_s.LEFTreg2_f.first) + VGet(0, 1.8f, 0), this->obj.frame(this->frame_s.LEFTreg2_f.first));
						if (pp.HitFlag) {
							this->pos_LEFTREG = VECTOR_ref(pp.HitPosition) - (this->obj.frame(this->frame_s.LEFTreg2_f.first) - this->obj.frame(this->frame_s.LEFTreg_f.first));
						}
						//*/
					}

					{
						//基準
						VECTOR_ref tgt_pt = this->pos_LEFTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->obj.frame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_LEFTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->obj.frame(this->frame_s.LEFTreg_f.first) - this->obj.frame(this->frame_s.LEFTfoot2_f.first)).size(), (this->obj.frame(this->frame_s.LEFTfoot2_f.first) - this->obj.frame(this->frame_s.LEFTfoot1_f.first)).size(), (this->obj.frame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFTfoot2_f.first) - this->obj.frame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

						//下腕
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.LEFTreg_f.first) - this->obj.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->obj.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
						//手
						this->obj.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
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
					this->pos_RIGHTREG = this->pos_RIGHTREG + (this->pos_tt - this->rec_HMD);
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_RIGHTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->obj.frame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_RIGHTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする


						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->obj.frame(this->frame_s.RIGHTreg_f.first) - this->obj.frame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTfoot2_f.first) - this->obj.frame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHTfoot2_f.first) - this->obj.frame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						//下腕
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHTreg_f.first) - this->obj.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->obj.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						//手
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
					}

					{
						/*
						auto pp = MAPPTs->map_col_line(this->obj.frame(this->frame_s.RIGHTreg2_f.first) + VGet(0, 1.8f, 0), this->obj.frame(this->frame_s.RIGHTreg2_f.first));
						if (pp.HitFlag) {
							this->pos_RIGHTREG = VECTOR_ref(pp.HitPosition) - (this->obj.frame(this->frame_s.RIGHTreg2_f.first) - this->obj.frame(this->frame_s.RIGHTreg_f.first));
						}
						*/
					}
					{
						//基準
						VECTOR_ref tgt_pt = this->pos_RIGHTREG;
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->obj.frame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
						//VECTOR_ref vec_a1L1 = (this->mat_RIGHTREG*this->mat.Inverse()).zvec()*-1.f;//x=0とする

						VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

						float cos_t = getcos_tri((this->obj.frame(this->frame_s.RIGHTreg_f.first) - this->obj.frame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTfoot2_f.first) - this->obj.frame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->obj.frame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
						VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
						//上腕
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHTfoot2_f.first) - this->obj.frame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
						//下腕
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->obj.frame(this->frame_s.RIGHTreg_f.first) - this->obj.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->obj.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
						//手
						this->obj.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
					}
				}
			}
			//手
			{}
			{
				this->anime_reload->reset();
				if (this->get_alive()) {
					//右手
					DrawPts->GetDevicePositionVR(DrawPts->get_hand1_num(), &this->pos_RIGHTHAND, &this->mat_RIGHTHAND);
					this->pos_RIGHTHAND = this->pos_RIGHTHAND + (this->pos_tt - this->rec_HMD);
					this->mat_RIGHTHAND = MATRIX_ref::Axis1(this->mat_RIGHTHAND.xvec()*-1.f, this->mat_RIGHTHAND.yvec(), this->mat_RIGHTHAND.zvec()*-1.f);
					this->mat_RIGHTHAND = this->mat_RIGHTHAND*MATRIX_ref::RotAxis(this->mat_RIGHTHAND.xvec(), deg2rad(-60));
					this->mat_RIGHTHAND =
						MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->blur_vec)*
						MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->recoil_vec)*
						this->mat_RIGHTHAND;//リコイル
					//左手
					DrawPts->GetDevicePositionVR(DrawPts->get_hand2_num(), &this->pos_LEFTHAND, &this->mat_LEFTHAND);
					this->pos_LEFTHAND = this->pos_LEFTHAND + (this->pos_tt - this->rec_HMD);
					this->mat_LEFTHAND = MATRIX_ref::Axis1(this->mat_LEFTHAND.xvec()*-1.f, this->mat_LEFTHAND.yvec(), this->mat_LEFTHAND.zvec()*-1.f);
					this->mat_LEFTHAND = this->mat_LEFTHAND*MATRIX_ref::RotAxis(this->mat_LEFTHAND.xvec(), deg2rad(-60));
				}
				//右手
				{
					//銃器
					if (this->get_alive()) {
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
						VECTOR_ref ans_;
						VECTOR_ref f_ = this->underhandguard.LEFT_pos();
						if (f_ != VGet(0, 0, 0)) {
							ans_ = f_;
						}
						f_ = this->foregrip.LEFT_pos();
						if (f_ != VGet(0, 0, 0)) {
							ans_ = f_;
						}
						{
							float dist_ = (this->pos_LEFTHAND - ans_).size();
							if (dist_ <= 0.1f && (!this->reloadf || !this->key_.have_magazine)) {
								this->LEFT_hand = true;
								this->pos_LEFTHAND = ans_;
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
		template<class Y2, class D2>
		void move_NOMAL(std::unique_ptr<Y2, D2>& MAPPTs, Chara& mine) {
			if (this->get_alive()) {
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
				if (this->get_alive()) {
					this->obj.SetMatrix(MATRIX_ref::Mtrans(this->pos_tt - this->rec_HMD));
					this->obj.SetFrameLocalMatrix(this->frame_s.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(this->frame_s.bodyg_f.second));
					this->obj.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
					this->obj.SetFrameLocalMatrix(this->frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(this->frame_s.body_f.second));

					if (this->reloadf || this->running() || this->anime_arm_check->per == 1.f) {
						mb_inv = MATRIX_ref::RotZ(this->body_zrad);
						m_inv = MATRIX_ref::RotZ(this->body_zrad)* MATRIX_ref::RotX(this->body_xrad)*mg_inv;
						this->obj.frame_reset(this->frame_s.bodyg_f.first);
						this->obj.frame_reset(this->frame_s.bodyb_f.first);
						this->obj.frame_reset(this->frame_s.body_f.first);
						this->obj.SetFrameLocalMatrix(this->frame_s.bodyg_f.first, mg_inv*this->obj.GetFrameLocalMatrix(this->frame_s.bodyg_f.first));
						this->obj.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, mb_inv*this->obj.GetFrameLocalMatrix(this->frame_s.bodyb_f.first));
						this->obj.SetFrameLocalMatrix(this->frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*this->obj.GetFrameLocalMatrix(this->frame_s.body_f.first));
					}
				}
				else {
					m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
					this->obj.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->pos_tt - this->rec_HMD));
				}
			}
			//頭部
			if (this->get_alive()) {
				//MATRIX_ref mmm = this->obj.GetFrameLocalMatrix(this->frame_s.body_f.first)*MATRIX_ref::Mtrans(this->obj.GetFrameLocalMatrix(this->frame_s.body_f.first).pos()).Inverse();

				this->obj.SetFrameLocalMatrix(this->frame_s.head_f.first, this->mat*m_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
				if (this->reloadf || this->running() || this->anime_arm_check->per == 1.f) {
					this->obj.frame_reset(this->frame_s.head_f.first);
				}
			}
			//足
			{
				auto ratio_t = 0.f;
				if (this->running()) {
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
				if (this->get_alive()) {
					float ani_walk = 0.f;//1
					float ani_run = 0.f;//2
					float ani_sit = 0.f;//7
					float ani_wake = 0.f;//11
					float ani_swalk = 0.f;//8
					if (this->running()) {
						ani_run = this->ratio_r;
					}
					else {
						if (!this->squat_on()) {
							ani_wake = 1.f - this->ratio_r;
							ani_walk = this->ratio_r;
						}
						else {
							ani_sit = 1.f - this->ratio_r;
							ani_swalk = this->ratio_r;
						}
						if (!this->ads_on()) {
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
					if (s.get_attaching()) {
						pv = VECTOR_ref(s.get_attach_frame()) + s.get_sight_frame();
						search_base(pv, &s);
					}
				}

				if (this->add_ypos == 0.f) {
					if (this->running()) {
						if (abs(sin(DX_PI_F*2.f*(this->anime_run->time / this->anime_run->alltime)*this->anime_run->per)) >= 0.8f && !this->audio.foot_.check()) {
							this->audio.foot_.play_3D(this->pos_gun, 15.f);
							this->audio.foot_.vol(255);
						}
					}
					else {
						if (!this->squat_on()) {
							if (abs(sin(DX_PI_F*2.f*(this->anime_walk->time / this->anime_walk->alltime)*this->anime_walk->per)) >= 0.8f && !this->audio.foot_.check()) {
								this->audio.foot_.play_3D(this->pos_gun, 5.f);
								this->audio.foot_.vol(128);
							}
						}
						else {
							if (abs(sin(DX_PI_F*2.f*(this->anime_swalk->time / this->anime_swalk->alltime)*this->anime_swalk->per)) >= 0.8f && !this->audio.foot_.check()) {
								this->audio.foot_.play_3D(this->pos_gun, 1.5f);
								this->audio.foot_.vol(64);
							}
						}
					}
				}
				else {
					//this->audio.foot_.stop();
				}
				//gunpos指定
				if (this->ads_on()) {
					easing_set(&this->gunpos, VGet(
						-0.035f - pv.x(),//右目で見るため
						-pv.y() + sin(DX_PI_F*2.f*(this->anime_walk->time / this->anime_walk->alltime))*this->anime_walk->per*0.001f, -0.315f
					)
						,
						std::min(0.75f + ((0.9f - 0.75f)*(this->per_all.weight - this->base.thisparts->per.weight) / 3.f), 0.935f)
					);
				}
				else if (this->running()) {
					easing_set(&this->gunpos, VGet(-0.15f, -0.1f - pv.y(), -0.25f), 0.9f);
				}
				else {
					easing_set(&this->gunpos,
						VGet(
							-0.15f,
							-0.075f - pv.y() + sin(DX_PI_F*2.f*(this->anime_walk->time / this->anime_walk->alltime))*this->anime_walk->per*0.001f*2.f,
							(this != &mine) ? -0.335f : -0.275f
						), 0.75f);
				}
			}
			//手
			{
				this->obj.frame_reset(this->frame_s.RIGHTarm1_f.first);
				this->obj.frame_reset(this->frame_s.RIGHTarm2_f.first);
				this->obj.frame_reset(this->frame_s.RIGHThand_f.first);
				this->obj.frame_reset(this->frame_s.LEFTarm1_f.first);
				this->obj.frame_reset(this->frame_s.LEFTarm2_f.first);
				this->obj.frame_reset(this->frame_s.LEFThand_f.first);
				//
				if (this->get_alive()) {
					if (this->sort_ing) {
						this->anime_arm_check->per = 1.f;
						this->anime_arm_check->update(false, 1.f);
						if (this->anime_arm_check->time == this->anime_arm_check->alltime) {
							if (!CheckSoundMem(this->audio.sort_magazine.get()) && !CheckSoundMem(this->audio.load_.get())) {
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
						if (this->running()) {
							if (this->reloadf && this->gun_stat_now->not_mags_EMPTY()) {
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
							if (this->reloadf && this->gun_stat_now->not_mags_EMPTY()) {
								this->anime_reload->per = 1.f;
								this->anime_reload->update(true, ((this->anime_reload->alltime / 30.f) / this->base.thisparts->reload_time));
							}
							else {
								this->anime_reload->reset();
								//右手
								{
									//視点を一時取得
									this->pos_HMD = (this->obj.frame(this->frame_s.RIGHTeye_f.first) + (this->obj.frame(this->frame_s.LEFTeye_f.first) - this->obj.frame(this->frame_s.RIGHTeye_f.first))*0.5f) - this->pos_tt;
									//銃器
									this->mat_gun =
										MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->blur_vec)*
										MATRIX_ref::RotVec2(VGet(0, 0, 1.f), this->recoil_vec)*
										this->mat;//リコイル
									this->pos_gun = this->get_pos() + (MATRIX_ref::Vtrans(this->gunpos, this->mat_gun) - this->rec_HMD);
									{
										int radd = 0;
										VECTOR_ref pos_g = this->pos_gun;
										MATRIX_ref mat_g = this->mat_gun;
										while (true) {
											this->base.obj.SetMatrix(mat_g * MATRIX_ref::Mtrans(pos_g));
											VECTOR_ref StartPos = this->base.RIGHT_pos();
											VECTOR_ref EndPos = this->base.mazzule_none_pos();
											this->mazzule.Setpos_parts(mat_g, 0);
											if (this->mazzule.get_mazzuletype() > 0) {
												EndPos = this->mazzule.mazzule_pos();
											}
											if (MAPPTs->map_col_line(StartPos, EndPos).HitFlag) {
												pos_g += this->mat_gun.zvec()*0.003f;
												mat_g = MATRIX_ref::RotX(deg2rad(10))* mat_g;//リコイル
											}
											else {
												break;
											}
											radd += 10;
											if (radd > 90) {
												break;
											}
										}
										easing_set(&this->rad_gun, float(radd), 0.9f);
										this->mat_gun = MATRIX_ref::RotX(deg2rad(this->rad_gun))* this->mat_gun;//リコイル
										if (radd != 0) {
											this->pos_gun = pos_g;
										}
									}
									this->Set_gun();
									//
									move_righthand(false, m_inv, false);
								}
								//左手
								{
									if (this->key_.have_magazine) {
										this->pos_LEFTHAND = VECTOR_ref(this->base.magf_pos()) + this->mat_gun.yvec()*-0.05f;
									}
									if (!this->reloadf || !this->key_.have_magazine || !this->key_.have_magazine) {
										VECTOR_ref f_ = this->underhandguard.LEFT_pos();
										if (f_ != VGet(0, 0, 0)) {
											this->pos_LEFTHAND = f_;
										}
										f_ = this->foregrip.LEFT_pos();
										if (f_ != VGet(0, 0, 0)) {
											this->pos_LEFTHAND = f_;
										}
									}

									this->LEFT_hand = (!this->reloadf || !this->key_.have_magazine);
									this->mat_LEFTHAND = this->mat_gun;
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
	public:
		//
		void Set(std::vector<GUNPARTs>& gun_data, const size_t& itr, MV1& body_, MV1& col_) {
			//変数
			{
				this->gunf = false;
				this->reloadf = false;
				this->LEFT_hand = false;
				this->recoil_vec = VGet(0, 0, 1.f);
				this->recoil_vec_res = this->recoil_vec;
				this->blur_vec = VGet(0, 0, 1.f);
				this->blur_vec_res = this->blur_vec;
				this->HP = this->HP_full;
				this->kill_f = false;
				this->kill_id = 0;
				this->kill_streak = 0;
				this->kill_time = 0.f;
				this->score = 0;
				this->kill_cnt = 0;
				this->death_id = 0;
				this->death_cnt = 0;
				this->per_all.recoil = 0.f;
				this->per_all.weight = 0.f;
			}
			//base
			add_parts(&gun_data[itr], EnumGunParts::PARTS_BASE, nullptr, 0);
			//身体
			{
				body_.DuplicateonAnime(&this->obj);
				//
				this->anime_walk = &this->obj.get_anime(1);
				this->anime_swalk = &this->obj.get_anime(8);
				this->anime_run = &this->obj.get_anime(2);
				//
				this->anime_sit = &this->obj.get_anime(7);
				this->anime_wake = &this->obj.get_anime(11);
				//
				this->anime_die_1 = &this->obj.get_anime(4);
				//
				this->anime_reload = &this->obj.get_anime(3);
				//
				this->anime_arm_run = &this->obj.get_anime(6);
				this->anime_arm_check = &this->obj.get_anime(10);
				//
				this->anime_hand_nomal = &this->obj.get_anime(0);
				this->anime_hand_trigger = &this->obj.get_anime(9);
				this->anime_hand_trigger_pull = &this->obj.get_anime(5);
				//
				this->frame_s.get_frame(this->obj, &this->head_hight);
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
				for (auto& a : this->base.obj.get_anime()) {
					a.reset();
				}
				this->gunanime_first = &this->base.obj.get_anime(6);
				this->gunanime_shot = &this->base.obj.get_anime(0);
				this->gunanime_shot_last = &this->base.obj.get_anime(1);
				this->gunanime_trigger = &this->base.obj.get_anime(2);
				this->gunanime_magcatch = &this->base.obj.get_anime(5);
				gunanime_sel.resize(this->base.thisparts->select.size());
				this->gunanime_sel[0] = &this->base.obj.get_anime(7);
				this->gunanime_sel[1] = &this->base.obj.get_anime(4);
			}
			//
			{
				this->gun_stat.resize(gun_data.size());
				for (auto& i : this->gun_stat) { i.Set(); }
				this->gun_stat_now = &gun_stat[this->base.thisparts->id_t];
			}
			for (auto& a : this->cart) { a.Set(); }				//薬莢
			for (auto& a : this->bullet) { a.Set(); }			//bullet
			this->audio.Duplicate(this->base.thisparts->audio);	//audio
		}
		//
		void set_gun(const VECTOR_ref& vec_t, const MATRIX_ref& mat_t) {
			this->pos_gun = vec_t;
			this->mat_gun = mat_t;
		}
		void set_mag() {
			this->mat_mag = this->mat_gun;
			this->pos_mag = this->base.mag2f_pos();
		}
		void Set_mag() {
			this->magazine.obj.SetMatrix(this->mat_mag* MATRIX_ref::Mtrans(this->pos_mag));
			if (this->magazine.get_type() == EnumGunParts::PARTS_MAGAZINE) {
				this->magazine.obj_ammo[0].SetMatrix(this->magazine.obj.GetMatrix() * MATRIX_ref::Mtrans(this->magazine.obj.frame(this->magazine.magazine_ammo_f[0].first) - this->magazine.obj.GetMatrix().pos()));
				this->magazine.obj_ammo[1].SetMatrix(this->magazine.obj.GetMatrix() * MATRIX_ref::Mtrans(this->magazine.obj.frame(this->magazine.magazine_ammo_f[1].first) - this->magazine.obj.GetMatrix().pos()));
			}
		}
		float Get_rad_chara() {
			VECTOR_ref vec = this->obj.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).zvec()*-1.f;
			return atan2f(vec.x(), vec.z());
		}
		void add_parts(GUNPARTs* partsptr, const size_t& type_t, g_parts* parents, size_t side) {
			if (partsptr != nullptr) {
				switch (type_t) {
				case EnumGunParts::PARTS_BASE:
				{
					this->base.attach_(this->per_all, *partsptr, type_t);
					break;
				}
				case EnumGunParts::PARTS_MAGAZINE:
				{
					this->magazine.attach_(this->per_all, *partsptr, type_t);
					break;
				}
				case EnumGunParts::PARTS_MAZZULE:
				{
					this->mazzule.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_GRIP:
				{
					this->grip.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_UPER_HGUARD:
				{
					this->uperhandguard.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_UNDER_HGUARD:
				{
					this->underhandguard.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_DUSTCOVER:
				{
					this->dustcover.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_STOCK:
				{
					this->stock.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_LASER:
				{
					this->laser.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_LIGHT:
				{
					this->light.attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_FOREGRIP:
				{
					this->foregrip.attach_(this->per_all, *partsptr, type_t, parents, side);
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
					this->mount_.back().attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				case EnumGunParts::PARTS_SIGHT:
				{
					if (parents != nullptr) {
						this->sight_.resize(this->sight_.size() + 1);
					}
					this->sight_.back().attach_(this->per_all, *partsptr, type_t, parents, side);
					break;
				}
				default:
					break;
				}
			}
		}
		void delete_parts(const size_t& type_t, const int& sight_mount = 0) {
			switch (type_t) {
			case EnumGunParts::PARTS_MAGAZINE:
			{
				this->delete_child(&this->magazine);
				this->magazine.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_BASE:
			{
				this->delete_child(&this->base);
				this->base.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_MAZZULE:
			{
				this->delete_child(&this->mazzule);
				this->mazzule.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_GRIP:
			{
				this->delete_child(&this->grip);
				this->grip.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_UPER_HGUARD:
			{
				this->delete_child(&this->uperhandguard);
				this->uperhandguard.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_UNDER_HGUARD:
			{
				this->delete_child(&this->underhandguard);
				this->underhandguard.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_DUSTCOVER:
			{
				this->delete_child(&this->dustcover);
				this->dustcover.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_STOCK:
			{
				this->delete_child(&this->stock);
				this->stock.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_LASER:
			{
				this->delete_child(&this->laser);
				this->laser.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_LIGHT:
			{
				this->delete_child(&this->light);
				this->light.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_FOREGRIP:
			{
				this->delete_child(&this->foregrip);
				this->foregrip.detach_(this->per_all);
				break;
			}
			case EnumGunParts::PARTS_SIGHT:
			{
				if (this->sight_.size() > 0) {
					this->delete_child(&this->sight_[std::clamp(int(this->sight_.size()) - 1, 0, std::max(0, sight_mount))]);
					if (this->sight_.size() > 0) {
						this->sight_[std::clamp(int(this->sight_.size()) - 1, 0, std::max(0, sight_mount))].detach_(this->per_all);
					}
					Check_empty_sight_slot();
				}
				break;
			}
			case EnumGunParts::PARTS_MOUNT:
			{
				if (this->mount_.size() > 0) {
					this->delete_child(&this->mount_[std::clamp(int(this->mount_.size()) - 1, 0, std::max(0, sight_mount))]);
					if (this->mount_.size() > 0) {
						this->mount_[std::clamp(int(this->mount_.size()) - 1, 0, std::max(0, sight_mount))].detach_(this->per_all);
					}
					Check_empty_mount_slot();
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

			this->pos_tt = this->spawn_pos;
			this->mat = this->spawn_mat;

			this->key_.ReSet_();

			this->add_ypos = 0.f;
			this->add_vec_buf.clear();
			this->Start_b = true;

			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->death_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;

			this->gun_stat_now->Set();
			this->gun_stat_now->magazine_insert(this->magazine.thisparts);		//マガジン+1(装填あり)
			this->gun_stat_now->magazine_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat_now->magazine_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat_now->magazine_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat_now->magazine_plus(this->magazine.thisparts);			//マガジン+1(装填無し)
			this->gun_stat_now->magazine_plus(this->magazine.thisparts);			//マガジン+1(装填無し)

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度
			std::for_each(this->wayp_pre.begin(), this->wayp_pre.end(), [](int& i) {i = 0; });
			this->gunf = false;
			this->gunanime_shot->reset();
			this->gunanime_shot_last->reset();
			this->gunanime_trigger->reset();
			this->gunanime_magcatch->reset();
			this->gunanime_first->reset();
			this->gunanime_sel[0]->reset();
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
			this->ReSet_waypoint(MAPPTs);
			this->gunanime_first->per = 1.f;
		}
		void Set_gun(void) {
			this->base.obj.SetMatrix(MATRIX_ref(this->mat_gun) * MATRIX_ref::Mtrans(this->pos_gun));
			{
				this->underhandguard.Setpos_parts(this->mat_gun, 0);
				this->uperhandguard.Setpos_parts(this->mat_gun, 0);
				this->grip.Setpos_parts(this->mat_gun, 0);
				this->mazzule.Setpos_parts(this->mat_gun, 0);
				this->dustcover.Setpos_parts(this->mat_gun, 0);
				this->stock.Setpos_parts(this->mat_gun, 0);
				this->foregrip.Setpos_parts(this->mat_gun, 0);
				this->light.Setpos_parts(this->mat_gun, 0);
				this->laser.Setpos_parts(this->mat_gun, 0);

				for (auto&m : this->mount_) {
					m.Setpos_parts(this->mat_gun, 0);
				}
				for (auto&s : this->sight_) {
					s.Setpos_parts(this->mat_gun, 0);
				}
			}
		}
		void calc_cart(const float& cart_rate_t) {
			//弾
			this->bullet[this->use_bullet].Put(&this->base.thisparts->ammo[0], this->base.mazzule_none_pos(), MATRIX_ref::Axis1(this->mat_gun.xvec()*-1.f, this->mat_gun.yvec()*-1.f, this->mat_gun.zvec()*-1.f));
			//薬莢
			this->cart[this->use_bullet].Put(this->base.thisparts->ammo[0].get_model(), this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first), (this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first + 1) - this->base.obj.frame(this->base.thisparts->frame_s.cate_f.first)).Norm()*2.5f / GetFPS(), this->mat_gun, cart_rate_t);
			//
			++this->use_bullet %= this->bullet.size();//次のIDへ
		}
		void calc_shot_effect(void) {
			switch (this->mazzule.get_mazzuletype())
			{
			case 0:
				this->effcs[ef_fire].set(this->mazzule.mazzule_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//ノーマル
				break;
			case 1:
				this->effcs[ef_fire2].set(this->mazzule.mazzule_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//サプ
				break;
			default:
				this->effcs[ef_fire].set(this->base.mazzule_none_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//何もついてない
				break;
			}
		}
		template<class Y2, class D2>
		void update_cart(std::unique_ptr<Y2, D2>& MAPPTs) {
			for (auto& a : this->cart) {
				if (a.Get_Flag()) {
					a.UpDate_pos();
					a.UpDate_fall(MAPPTs->map_col_line(a.Get_pos() + VGet(0, 1.f, 0), a.Get_pos() - VGet(0, 0.008f, 0)), *this);
				}
			}
		}
		void Set_select() {
			this->base.thisparts->Set_gun_select(this->gunanime_sel, this->gun_stat_now->get_select());
		}
		void Set_Draw_bullet(void) {
			for (auto& a : this->bullet) {
				a.Set_Draw();
			}
		}
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
		void Draw_chara(void) {
			if (this->Start_a) {
				this->obj.DrawModel();
				//this->col.DrawModel();
				Draw_gun();
			}
		}
		void Draw_gun() {
			//
			this->base.obj.DrawModel();
			for (auto& a : this->cart) {
				a.Draw();
			}
			//
			this->mazzule.Draw();
			this->grip.Draw();
			this->uperhandguard.Draw();
			this->underhandguard.Draw();
			for (auto&m : mount_) {
				m.Draw();
			}
			this->dustcover.Draw();
			this->stock.Draw();
			this->foregrip.Draw();
			this->light.Draw();
			this->laser.Draw();
			for (auto&s : sight_) {
				s.Draw();
			}
			if ((!this->reloadf || this->key_.have_magazine) && this->gun_stat_now->not_mags_EMPTY()) {
				this->magazine.Draw(this->gun_stat_now->get_ammo_cnt());
			}
		}
		void Draw_ammo(void) {
			for (auto& a : this->bullet) {
				a.Draw();
			}
		}
		template<class Y2, class D2>
		void Draw_laser(std::vector<Chara>&chara, std::unique_ptr<Y2, D2>& MAPPTs, GraphHandle&light_pic) {
			if (this->laser.get_attaching()) {
				VECTOR_ref StartPos = this->laser.source_frame_pos();
				VECTOR_ref EndPos = StartPos - this->base.obj.GetMatrix().zvec()*100.f;
				MAPPTs->map_col_nearest(StartPos, &EndPos);
				for (auto& tgt : chara) {
					if (&tgt == this) {
						continue;
					}
					for (int i = 0; i < 3; i++) {
						auto q = tgt.col.CollCheck_Line(StartPos, EndPos, -1, i);
						if (q.HitFlag) {
							EndPos = q.HitPosition;
						}
					}
				}
				SetUseLighting(FALSE);
				SetFogEnable(FALSE);

				DXDraw::Capsule3D(StartPos, EndPos, 0.001f, GetColor(255, 0, 0), GetColor(255, 255, 255));
				DrawSphere3D(EndPos.get(), std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.05f), 6, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
			}
			if (this->light.get_attaching()) {
				VECTOR_ref StartPos = this->light.source_frame_pos();
				DrawBillboard3D(StartPos.get(), 0.5f, 0.5f, 0.1f, 0.f, light_pic.get(), TRUE);
			}
		}
		//
		void Delete_chara(void) {
			//パーツリセット
			delete_parts(EnumGunParts::PARTS_BASE);
			//
			this->obj.Dispose();
			this->col.Dispose();
			//
			delete_parts(EnumGunParts::PARTS_MAGAZINE);
			//
			for (auto& a : this->cart) {
				a.Dispose();
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
		//
		void Set_shot(float&rate) {
			this->gunanime_shot->per = 1.f;
			if (this->gunf) {
				this->gunanime_shot->update(true, rate*2.f*this->gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
				if (this->gunanime_shot->time == 0.f) {
					this->gunf = false;
				}
			}
		}
		const VECTOR_ref Set_HP_UI(const VECTOR_ref& cam_pos) {
			VECTOR_ref p;
			if ((this->HP - int(this->HP_r)) <= -2) {
				p = ConvWorldPosToScreenPos((this->get_head_frame() + VGet(0, 0.3f + 2.7f*std::max((this->get_head_frame() - cam_pos).size(), 1.f) / 100.f, 0)).get());
			}
			else {
				p.z(-1.f);
			}
			return p;
		}
		//視界外か否かを判断
		void Check_CameraViewClip() {
			this->Start_a = true;
			auto ttt = this->obj.GetMatrix().pos();
			if (CheckCameraViewClip_Box((ttt + VGet(-0.3f, 0, -0.3f)).get(), (ttt + VGet(0.3f, 1.8f, 0.3f)).get())) {
				this->Start_a = false;
				this->Start_b = true;
				return;
			}
		}
		template<class Y, class D>
		void Check_CameraViewClip(std::unique_ptr<Y, D>& MAPPTs) {
			this->Start_a = true;
			auto ttt = this->obj.GetMatrix().pos();
			if (CheckCameraViewClip_Box((ttt + VGet(-0.3f, 0, -0.3f)).get(), (ttt + VGet(0.3f, 1.8f, 0.3f)).get())) {
				this->Start_a = false;
				this->Start_b = true;
				return;
			}
			//*
			if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VGet(0, 1.8f, 0)).HitFlag && MAPPTs->map_col_line(GetCameraPosition(), ttt + VGet(0, 0.f, 0)).HitFlag) {
				this->Start_a = false;
				this->Start_b = true;
				return;
			}
			//*/
			//*
			//念のため
			if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VGet(0, 1.2f, 0)).HitFlag && MAPPTs->map_col_line(GetCameraPosition(), ttt + VGet(0, 0.6f, 0)).HitFlag) {
				this->Start_a = false;
				this->Start_b = true;
				return;
			}
			//*/
		}
		// UI向けにヒット部分を表示
		void Draw_Hit_UI(GraphHandle& hit_Graph, const bool& ismine) {
			for (auto& a : this->bullet) {
				if (255.f*a.hit_alpha >= 10) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f*a.hit_alpha));
					hit_Graph.DrawRotaGraph(a.hit_window_x, a.hit_window_y, a.hit_alpha*0.5f, 0.f, true);//(ismine ? 1.f : 0.5f)
				}
			}
		}
		//
	public:
		//
		template<class Y, class D>
		void UpDate(
			std::unique_ptr<Y, D>& MAPPTs,
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts,
			std::vector<Hit>& hit_obj, size_t& hit_buf,
			const bool& playing, const float& fov_per,
			std::vector <Meds>& meds_data, std::vector<Items>& item,
			std::vector<Chara>&chara, Chara&mine, const bool& use_vr) {
				const auto fps_ = GetFPS();
				//
				{
					easing_set(&this->HP_r, float(this->HP), 0.95f);
					easing_set(&this->got_damage_f, 0.f, 0.95f);
					if (!this->get_alive()) {
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
							this->obj.frame_reset(this->frame_s.bodyg_f.first);
							this->obj.frame_reset(this->frame_s.bodyb_f.first);
							this->obj.frame_reset(this->frame_s.body_f.first);
							this->obj.frame_reset(this->frame_s.head_f.first);
							this->obj.frame_reset(this->frame_s.RIGHTarm1_f.first);
							this->obj.frame_reset(this->frame_s.RIGHTarm1_f.first);
							this->obj.frame_reset(this->frame_s.RIGHTarm2_f.first);
							this->obj.frame_reset(this->frame_s.RIGHTarm2_f.first);
							this->obj.frame_reset(this->frame_s.RIGHThand_f.first);
							this->obj.frame_reset(this->frame_s.LEFTarm1_f.first);
							this->obj.frame_reset(this->frame_s.LEFTarm1_f.first);
							this->obj.frame_reset(this->frame_s.LEFTarm2_f.first);
							this->obj.frame_reset(this->frame_s.LEFTarm2_f.first);
							this->obj.frame_reset(this->frame_s.LEFThand_f.first);
							magrelease_t(item, 0);//マガジン排出
						}
						this->death_timer += 1.f / fps_;
						if (this->death_timer >= 3.f) {
							this->death_timer = 0.f;
							this->anime_die_1->reset();
							this->spawn(this->spawn_pos, this->spawn_mat);
							ReSet_waypoint(MAPPTs);
							this->gunanime_shot->per = 1.f;
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
				//座標取得
				if (this == &mine) {
					if (DrawPts->tracker_num.size() > 0) {
						auto& ptr_ = (*DrawPts->get_device())[DrawPts->tracker_num[0]];
						DrawPts->GetDevicePositionVR(DrawPts->tracker_num[0], &this->pos_WAIST, &this->mat_WAIST);
						if (this->Start_c || (ptr_.turn && ptr_.now) != this->oldv_1_2) {
							this->pos_WAIST_rep = VGet(this->pos_WAIST.x(), 0.f, this->pos_WAIST.z());
						}
						this->oldv_1_2 = ptr_.turn && ptr_.now;
						this->pos_WAIST = this->pos_WAIST - this->pos_WAIST_rep;
					}
				}
				//プレイヤー操作
				{
					if (this == &mine) {
						//mine
						if (use_vr) {
							this->operation_VR(!playing, DrawPts);
						}
						else {
							this->operation_NOMAL(!playing, fov_per);
						}
					}
					else {
						//cpu
						this->operation_NPC(MAPPTs, chara, !playing);
					}
					//common
					this->operation_2();
				}
				//壁その他の判定
				this->wall_col(MAPPTs, mine, use_vr);
				//obj演算
				if (use_vr && (this == &mine)) {
					this->move_VR(/*MAPPTs,*/ DrawPts);
				}
				else {
					this->move_NOMAL(MAPPTs, mine);
				}
				//obj演算
				this->obj.work_anime();
				//col演算
				this->frame_s.copy_frame(this->obj, this->colframe_, &this->col);
				this->col.work_anime();
				this->col.RefreshCollInfo(-1, 0);
				this->col.RefreshCollInfo(-1, 1);
				this->col.RefreshCollInfo(-1, 2);
				//
				if (!(use_vr && (this == &mine))) {
					this->calc_hmd();							//視点取得
					this->calc_gun();							//銃器
				}
				//射撃関連
				this->calc_shot(MAPPTs, DrawPts, chara, hit_obj, hit_buf, meds_data, item);
				//アイテム拾う
				this->canget_gunitem = false;
				this->canget_magitem = false;
				this->canget_meditem = false;
				for (auto& g : item) {
					g.Get_item_2(*this, MAPPTs);
				}
				//物理演算、アニメーション
				{
					if (this->Start_c) {
						this->obj.PhysicsResetState();
						this->Start_c = false;
					}
					else  if (this->Start_b) {
						this->obj.PhysicsResetState();
						this->Start_b = false;
					}
					else {
						this->obj.PhysicsCalculation(1000.f / GetFPS());
					}
				}
				//
				if (this == &mine) {
					//レティクル
					for (auto&s : this->sight_) {
						s.Set_reticle();
					}
					//ライト
					this->light.Set_LightHandle(this->base.obj.GetMatrix().zvec()*-1.f);
				}
		}
		//カメラ指定
		template<class Y3, class D3>
		void Set_cam(std::unique_ptr<Y3, D3>& MAPPTs, cam_info& camera_main, std::vector<Chara>&chara, const float& fov_, const bool& use_vr) {
			if (this->get_alive()) {
				auto v_x = this->blur_vec;
				v_x.y(v_x.y() / 3.f);
				v_x = v_x.Norm();

				auto v_z = this->recoil_vec;
				v_z.y(v_z.y()*2.f / 3.f);
				v_z = v_z.Norm();

				auto mat_T =
					MATRIX_ref::RotVec2(VGet(0, 0, 1.f), v_x)*
					MATRIX_ref::RotVec2(VGet(0, 0, 1.f), v_z)*
					this->mat;//リコイル

				auto ppsh = MATRIX_ref::Vtrans(VGet(-0.035f, 0, 0), mat_T);
				if (use_vr) {
					mat_T = this->mat;
					ppsh.clear();
				}
				camera_main.set_cam_pos(this->get_pos() + ppsh, this->get_pos() + ppsh + mat_T.zvec()*(use_vr ? 1.f : -1.f), mat_T.yvec());
				if (this->ads_on()) {
					easing_set(&camera_main.fov, deg2rad(25),

						std::min(0.8f + ((0.9f - 0.8f)*(this->per_all.weight - this->base.thisparts->per.weight) / 3.f), 0.925f)

					);
				}
				else {
					easing_set(&camera_main.fov, fov_, 0.9f);
				}
			}
			else {
				//デスカメラ
				easing_set(&camera_main.camvec, chara[this->death_id].get_pos(), 0.9f);
				auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
				easing_set(&camera_main.campos, this->get_pos() + VGet(-5.f*sin(rad), 2.f, -5.f*cos(rad)), 0.9f);
				camera_main.camup = VGet(0, 1.f, 0);
				MAPPTs->map_col_nearest(camera_main.camvec, &camera_main.campos);
				easing_set(&camera_main.fov, fov_, 0.9f);
			}
		}
	public:
	};
	//アイテム
	class Items {
	private:
		//共通
		VECTOR_ref pos;
		VECTOR_ref add_vec;
		MATRIX_ref mat;
		MV1 obj;
		//銃専用パラメーター
		//マガジン専用パラメーター
		//治療キット専用パラメーター
	private:
		//共通
		float del_timer = 0.f;
		size_t id_t = 0;
		//銃専用パラメーター
		GUNPARTs* ptr_gun = nullptr;
		GUNPARTs gun;
		//マガジン専用パラメーター
		GUNPARTs* ptr_mag = nullptr;
		GUNPARTs magazine;
		//治療キット専用パラメーター
		Meds* ptr_med = nullptr;
		Meds medkit;
	public:
		auto& get_id_t() { return id_t; }
		auto& get_ptr_gun() { return ptr_gun; }
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
	public:
		//mag
		void Set_item_4(const size_t& id, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
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
		void Set_item_3_(const size_t& dnm, const size_t& id, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
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
		bool Set_item_2_(const size_t& dnm, GUNPARTs*magdata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			if (this->ptr_gun == nullptr && this->ptr_mag == nullptr && this->ptr_med == nullptr) {
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
		void Set_item_3(const size_t& id, Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->id_t = id;
			this->Set_item(meddata, pos_, add_, mat_);
		}
		bool Set_item_2(Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			if (this->ptr_gun == nullptr && this->ptr_mag == nullptr && this->ptr_med == nullptr) {
				this->Set_item(meddata, pos_, add_, mat_);
				return true;
			}
			return false;
		}
		void Set_item(Meds*meddata, const VECTOR_ref& pos_, const VECTOR_ref& add_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add_vec = add_;
			this->mat = mat_;
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.get_model().Duplicate();
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
		template<class Y2, class D2>
		void Get_item_2(Chara& chara, std::unique_ptr<Y2, D2>& MAPPTs) {
			VECTOR_ref StartPos = chara.get_pos_LEFTHAND();
			VECTOR_ref EndPos = StartPos - chara.get_mat_LEFTHAND().zvec()*2.6f;
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
					chara.canget_id = this->id_t;
					chara.get_canget_mag() = this->ptr_mag->mod.get_name();
					if (chara.getmagazine_push() && this->magazine.cap != 0 && (this->ptr_mag->ammo[0].get_name() == this->magazine.ammo[0].get_name())) {
						chara.sort_f = false;
						chara.gun_stat_now->magazine_plus(&(this->magazine));
						if (chara.gun_stat_now->get_magazine_in().size() == 1) {
							chara.reloadf = true;
						}
						this->Delete_item();
					}
				}
			}
			//
			if (this->ptr_med != nullptr) {
				chara.get_canget_meditem() |= zz;
				if (zz) {
					chara.canget_id = this->id_t;
					chara.get_canget_med() = this->ptr_med->mod.get_name();
					if (chara.getmagazine_push()) {
						chara.HP = std::clamp<int>(chara.HP + this->ptr_med->repair, 0, chara.HP_full);
						this->Delete_item();
					}
				}
			}
		}
		void Draw_item(void) {
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
				if (chara.get_canget_gunitem() && chara.canget_id == this->id_t) {
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
				if (chara.get_canget_magitem() && chara.canget_id == this->id_t) {
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
				if (chara.get_canget_meditem() && chara.canget_id == this->id_t) {
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
		void Delete_item(void) {
			this->ptr_gun = nullptr;
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->obj.Dispose();
		}
		bool Delete_mag(void) {
			if (this->ptr_mag != nullptr && this->del_timer <= 0.f) {
				this->Delete_item();
				return true;
			}
			return false;
		}
	};
};
//