#pragma once
#define FRAME_RATE 90.f
//リサイズ
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)
//
static const size_t max_bullet{ 32 };
//
enum Effect {
	ef_fire,	//発砲炎
	ef_reco,	//小口径跳弾
	ef_smoke,	//銃の軌跡
	ef_gndsmoke,//地面の軌跡
	ef_fire2,	//発砲炎
	ef_hitblood,//血しぶき
	ef_greexp,	//グレ爆発
	effects,	//読み込む
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
	PARTS_LAM,
	PARTS_MOUNT_BASE,
	PARTS_MOUNT,
	PARTS_SIGHT,
	PARTS_FOREGRIP,
	PARTS_NUM,
};
enum EnumAttachPoint {
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
enum EnumSELECTER {
	SELECT_SEMI,
	SELECT_FULL,
	SELECT_3B,
	SELECT_2B,
	SELECT_NUM,
};
enum EnumSELECT_LAM {
	SELECTLAM_LASER,
	SELECTLAM_LIGHT,
	SELECTLAM_NUM,
};
//option
class OPTION {
public:
	int grass_level = 4;
	bool DoF = false;
	bool Bloom = false;
	bool Shadow = false;
	bool useVR{ true };
	bool SSAO{ true };
	float Fov = 45.f;
	bool Vsync = false;
	OPTION(void)  noexcept {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		grass_level = std::clamp<int>(getparams::_int(mdata), 0, 4);
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
	~OPTION(void) noexcept {
	}
};
//キーバインド
class key_bind {
private:
	class keyhandle;
	class keys {
	public:
		int mac = 0, px = 0, py = 0;
		char onhandle[256] = "", offhandle[256] = "";
	};
	class key_pair {
	public:
		int first = 0;
		std::string second;
		bool isalways = false;
		switchs on_off;
		keyhandle* use_handle{ nullptr };
		int use_mode = 0;
		bool get_key(int id) {
			switch (id) {
				//キー
			case 0:
				return CheckHitKey(this->first) != 0;
			case 1:
				on_off.GetInput(CheckHitKey(this->first) != 0);
				return on_off.on();
			case 2:
				on_off.GetInput(CheckHitKey(this->first) != 0);
				return on_off.trigger();
				//マウス
			case 3:
				return (GetMouseInput() & this->first) != 0;
			case 4:
				on_off.GetInput((GetMouseInput() & this->first) != 0);
				return on_off.on();
			case 5:
				on_off.GetInput((GetMouseInput() & this->first) != 0);
				return on_off.trigger();
			default:
				return CheckHitKey(this->first) != 0;
			}
		}
	};
	class keyhandle {
	public:
		keys key;
		GraphHandle onhandle, offhandle;
		key_pair* use_part{ nullptr };
	};
	FontHandle font24;
	int font24size = 24;
	std::vector<keyhandle> keyg;
	std::vector<keyhandle> keyg2;

	float F1_f = 0.0f;
	float noF1_f = 0.0f;
	GraphHandle keyboad;
	GraphHandle mousehandle;
public:
	std::vector<key_pair> key_use_ID;
	std::vector<key_pair> mouse_use_ID;
	//
	bool get_key_use(int id_t) {
		key_use_ID[id_t].isalways = true;
		return key_use_ID[id_t].get_key(key_use_ID[id_t].use_mode);
	}
	bool get_mouse_use(int id_t) {
		mouse_use_ID[id_t].isalways = true;
		return mouse_use_ID[id_t].get_key(mouse_use_ID[id_t].use_mode);
	}
	//
	key_bind(void) noexcept {
		SetUseASyncLoadFlag(FALSE);
		font24 = FontHandle::Create(font24size, DX_FONTTYPE_EDGE);
		mousehandle = GraphHandle::Load("data/key/mouse.png");
		SetTransColor(0, 255, 0);
		keyboad = GraphHandle::Load("data/key/keyboad.png");
		SetTransColor(0, 0, 0);
		//
		{
			key_pair tmp_k;
			tmp_k.first = KEY_INPUT_W;
			tmp_k.second = "前進";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = KEY_INPUT_S;
			tmp_k.second = "後退";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = KEY_INPUT_D;
			tmp_k.second = "右歩き";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//2
			tmp_k.first = KEY_INPUT_A;
			tmp_k.second = "左歩き";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//3
			tmp_k.first = KEY_INPUT_Q;
			tmp_k.second = "左リーン";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//4
			tmp_k.first = KEY_INPUT_E;
			tmp_k.second = "右リーン";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//5
			tmp_k.first = KEY_INPUT_R;
			tmp_k.second = "リロード";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//6
			tmp_k.first = KEY_INPUT_F;
			tmp_k.second = "アイテム取得";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//7
			tmp_k.first = KEY_INPUT_G;
			tmp_k.second = "グレネード投擲";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//8
			tmp_k.first = KEY_INPUT_C;
			tmp_k.second = "しゃがみ";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//9
			tmp_k.first = KEY_INPUT_O;
			tmp_k.second = "タイトル画面に戻る";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//10
			tmp_k.first = KEY_INPUT_ESCAPE;
			tmp_k.second = "強制終了";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//11
			tmp_k.first = KEY_INPUT_Z;
			tmp_k.second = "マガジン整理";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//12
			tmp_k.first = KEY_INPUT_LSHIFT;
			tmp_k.second = "走る";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//13
			tmp_k.first = KEY_INPUT_SPACE;
			tmp_k.second = "ジャンプ";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//14
			tmp_k.first = KEY_INPUT_LCONTROL;
			tmp_k.second = "視点切替";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//15
			tmp_k.first = KEY_INPUT_F1;
			tmp_k.second = "キー案内";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//16
			tmp_k.first = KEY_INPUT_V;
			tmp_k.second = "眺める";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//17
			tmp_k.first = KEY_INPUT_P;
			tmp_k.second = "ポーズ";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//18
			tmp_k.first = KEY_INPUT_H;
			tmp_k.second = "治療キット排出";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//19
			//
			tmp_k.first = MOUSE_INPUT_LEFT;
			tmp_k.second = "射撃";
			tmp_k.use_mode = 3;
			this->mouse_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = MOUSE_INPUT_MIDDLE;
			tmp_k.second = "セレクター切替";
			tmp_k.use_mode = 5;
			this->mouse_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = MOUSE_INPUT_RIGHT;
			tmp_k.second = "エイム";
			tmp_k.use_mode = 3;
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
	const auto Esc_key(void) noexcept { return this->key_use_ID[11].get_key(0); }
	//
	void reSet_isalways(void) noexcept {
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
	void draw(void) noexcept {
		auto tmp_f1 = this->key_use_ID[16].get_key(1);
		easing_set(&F1_f, float(tmp_f1), 0.9f);
		noF1_f = std::max(noF1_f - 1.f / FPS, 0.f);
		//インフォ
		if (F1_f > 0.1f) {
			int xp_t = 100, yp_t = 300;
			int xp_sk = xp_t, yp_sk = yp_t, y_size_k = 48;
			int xp_s = 1500, yp_s = 200, y_size = 32;
			//背景
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*F1_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			if (F1_f > 0.9f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				keyboad.DrawExtendGraph(0, 0, 1920, 1080, true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			//前面
			if (F1_f > 0.9f) {
				//キーボード＋マウス全部
				{
					for (auto& m : this->keyg) {
						if (m.use_part != nullptr) {
							xp_sk = xp_t + m.key.px;
							yp_sk = yp_t + m.key.py;
							if (m.use_part->get_key(0)) {//keyboad
								m.onhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - 4) / 26.f, 0.f, false);
							}
							else {
								m.offhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - 4) / 26.f, 0.f, false);
							}
						}
					}
					int yss = 0;
					xp_sk = 1100;
					yp_sk = 800;
					for (auto& m : this->keyg2) {
						if (m.use_part != nullptr) {
							if (m.use_part->get_key(3)) {
								m.onhandle.GetSize(nullptr, &yss);
								m.onhandle.DrawRotaGraph(xp_sk, yp_sk, float(256) / yss, 0.f, true);
							}
							else {
								m.offhandle.GetSize(nullptr, &yss);
								m.offhandle.DrawRotaGraph(xp_sk, yp_sk, float(256) / yss, 0.f, true);
							}
						}
					}
				}
				//詳細
				{
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
			}
		}
		//常時表示
		if (!tmp_f1) {
			if (noF1_f >= 0.1f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*std::clamp(noF1_f, 0.f, 1.f)));
				int xp_s = 1920 - 700, yp_s = 1080 - 28, x_size = 26, y_size = 24;
				int xss = 0, yss = 0;
				for (auto& i : this->key_use_ID) {
					if (i.isalways) {
						for (auto& m : this->keyg) {
							if (m.key.mac == i.first) {
								if (i.get_key(0)) {
									noF1_f = 3.f;
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
									noF1_f = 3.f;
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
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			else {
				for (auto& i : this->key_use_ID) {
					if (i.isalways) {
						for (auto& m : this->keyg) {
							if (m.key.mac == i.first) {
								if (i.get_key(0)) {
									noF1_f = 3.f;
								}
							}
						}
					}
				}
				for (auto& i : this->mouse_use_ID) {
					if (i.isalways) {
						for (auto& m : this->keyg2) {
							if (m.key.mac == i.first) {
								if (i.get_key(3)) {
									noF1_f = 3.f;
								}
							}
						}
					}
				}
			}
		}
		else {
			noF1_f = 3.f;
		}
		//
	}
	void draw_botsu(void) noexcept {
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
	}
	//
};
//キーバインド
class pause_menu {
private:
	std::shared_ptr<key_bind> KeyBind{ nullptr };

	FontHandle font24;
	int font24size = 24;

	float P_f = 0.0f;
public:
	//
	pause_menu(std::shared_ptr<key_bind>& KeyBind_t) noexcept {
		KeyBind = KeyBind_t;
		SetUseASyncLoadFlag(FALSE);
		font24 = FontHandle::Create(font24size, DX_FONTTYPE_EDGE);
	}
	//
	const auto Pause_key(void) noexcept { return KeyBind->key_use_ID[18].get_key(1); }
	//
	bool Update(void) noexcept {
		KeyBind->key_use_ID[18].isalways = true;
		KeyBind->key_use_ID[10].isalways = KeyBind->key_use_ID[18].on_off.on();

		SetMouseDispFlag(TRUE);

		bool selend = true;
		//強制帰還はポーズメニューで
		if (KeyBind->key_use_ID[10].get_key(0)) {
			KeyBind->key_use_ID[18].on_off.first = false;
			selend = false;
		}
		return selend;
	}
	//
	void draw(void) noexcept {
		auto tmp_P = KeyBind->key_use_ID[18].on_off.on();
		easing_set(&P_f, float(tmp_P), 0.9f);
		//インフォ
		if (P_f > 0.1f) {
			int yp_t = 100;
			//背景
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*P_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			if (P_f > 0.9f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				//背景画像
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			//前面
			if (P_f > 0.9f) {
				yp_t = 100;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "オプション", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "Pキーで戦闘に戻る", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "Oキーで強制帰還", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
			}
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
	float get_timer(void) const noexcept { return timer; }
	float get_Ready(void) const noexcept { return Ready; }
	bool get_Start(void) const noexcept { return Ready <= 0.f; }
	bool get_Playing(void) const noexcept { return get_Start() && !get_end(); }
	bool get_end(void) const noexcept { return timer <= 0.f; }
	void Set(void) noexcept {
		Ready = 3.0f;
		timer = 180.f;
	}
	void UpDate(void) noexcept {
		if (get_Start()) {
			timer -= 1.f / FPS;
		}
		else {
			Ready -= 1.f / FPS;
		}
	}
};
//
class save_c {
public:
	size_t cang_ = 0;						//パーツ選択
	EnumGunParts type_ = PARTS_NONE;			//パーツの種類
	EnumAttachPoint pt_cat_ = POINTS_NONE;	//ベースパーツの場所
	EnumGunParts pt_type_ = PARTS_NONE;		//ベースパーツの種類
	size_t pt_sel_ = 0;						//ベースパーツの番号(マウントなど)
};
//
class shaders {
public:
	int pshandle{ -1 }, vshandle{ -1 };
	int pscbhandle{ -1 };
	int pscbhandle2{ -1 };

	void Init(std::string vs, std::string ps) {
		this->vshandle = LoadVertexShader(("shader/" + vs).c_str());	// 頂点シェーダーバイナリコードの読み込み
		this->pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		this->pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
		this->pshandle = LoadPixelShader(("shader/" + ps).c_str());		// ピクセルシェーダーバイナリコードの読み込み
	}
	void set_dispsize(int dispx, int dispy) {
		FLOAT2 *dispsize = (FLOAT2 *)GetBufferShaderConstantBuffer(this->pscbhandle);			// ピクセルシェーダー用の定数バッファのアドレスを取得
		dispsize->u = float(dispx);
		dispsize->v = float(dispy);
		UpdateShaderConstantBuffer(this->pscbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
	}
	void set_param(float param1, float param2, float param3, float param4) {
		FLOAT4 *f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->pscbhandle2);			// ピクセルシェーダー用の定数バッファのアドレスを取得
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->pscbhandle2);							// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
	}
	void draw(VERTEX3DSHADER Screen_vertex[]) {
		SetUseVertexShader(this->vshandle);		// 使用する頂点シェーダーをセット
		SetUsePixelShader(this->pshandle);		// 使用するピクセルシェーダーをセット
		MV1SetUseOrigShader(TRUE);
		DrawPolygon3DToShader(Screen_vertex, 2);// 描画
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);					// 使用する頂点シェーダーをセット
		SetUsePixelShader(-1);					// 使用するピクセルシェーダーをセット
	}
	void draw_lamda(std::function<void()> doing) {
		SetUseVertexShader(this->vshandle);		// 使用する頂点シェーダーをセット
		SetUsePixelShader(this->pshandle);		// 使用するピクセルシェーダーをセット
		MV1SetUseOrigShader(TRUE);
		doing();
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);					// 使用する頂点シェーダーをセット
		SetUsePixelShader(-1);					// 使用するピクセルシェーダーをセット
	}
};
//
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
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
		std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム
		std::array<frames, 3> LEFT_mag_frame;	//左手座標(マガジン保持時)
		std::array<frames, 2> magazine_f;		//マガジンフレーム
	public:
		auto& get_name() const noexcept { return name; }
		auto& get_path() const noexcept { return path; }
		auto& get_model() const noexcept { return model; }
		const frames& getmagazine_ammo_f(int i) const noexcept { return magazine_ammo_f[i]; }
		const frames& getLEFT_mag_frame(int i) const noexcept { return LEFT_mag_frame[i]; }
		const frames& getmagazine_f(int i) const noexcept { return magazine_f[i]; }
		void Ready(std::string path_, std::string named) noexcept {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/model.mv1", &this->model, true);
			MV1::Load(this->path + "/col.mv1", &this->col, true);
		}
		int mdata = -1;
		void Set_(std::function<void()> doing) {
			this->model.SetMatrix(MGetIdent());
			for (int i = 0; i < int(this->model.frame_num()); ++i) {
				std::string p = this->model.frame_name(i);
				if (p == std::string("ammo1")) {
					this->magazine_ammo_f[0] = { int(i),this->model.frame(i) };
				}
				if (p == std::string("ammo2")) {
					this->magazine_ammo_f[1] = { int(i),this->model.frame(i) };
				}
				if (p == std::string("LEFT_mag")) {
					this->LEFT_mag_frame[0] = { int(i),this->model.frame(i) };
					this->LEFT_mag_frame[1] = { int(i + 1),this->model.GetFrameLocalMatrix(i + 1).pos() };
					this->LEFT_mag_frame[2] = { int(i + 2),this->model.GetFrameLocalMatrix(i + 2).pos() };
				}
				if (p == std::string("magazine_fall")) {
					this->magazine_f[0] = { int(i),this->model.frame(i) };
					this->magazine_f[1] = { int(i + 1),this->model.GetFrameLocalMatrix(i + 1).pos() };
				}
			}
			this->mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			doing();
			FileRead_close(mdata);
		}
	};
protected:
	typedef std::pair<size_t, float> pair_hit;

	class damage_rad {
	public:
		float alpfa{ 1.f };
		float rad{ 0.f };
	};

	//キャラ用オーディオ
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
		SoundHandle explosion;
		//
		SoundHandle voice_damage;
		SoundHandle voice_death;
		SoundHandle voice_breath;
		SoundHandle voice_breath_run;
		//
		void Set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot_path = "data/audio/chara/shot_" + getparams::_str(mdata) + ".wav";
			shot			 = SoundHandle::Load(shot_path);
			slide_path = "data/audio/chara/slide_" + getparams::_str(mdata) + ".wav";
			slide			 = SoundHandle::Load(slide_path);
			trigger_path = "data/audio/chara/trigger_" + getparams::_str(mdata) + ".wav";
			trigger			 = SoundHandle::Load(trigger_path);
			magazine_down	 = SoundHandle::Load("data/audio/chara/mag_down_" + getparams::_str(mdata) + ".wav");
			magazine_Set	 = SoundHandle::Load("data/audio/chara/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down		 = SoundHandle::Load("data/audio/chara/case_2.wav");
			load_			 = SoundHandle::Load("data/audio/chara/load.wav");
			sort_magazine	 = SoundHandle::Load("data/audio/chara/sort.wav");
			foot_			 = SoundHandle::Load("data/audio/chara/foot_sand.wav");
			explosion		 = SoundHandle::Load("data/audio/chara/explosion.wav");

			voice_damage		 = SoundHandle::Load("data/audio/voice/damage.wav");
			voice_death			 = SoundHandle::Load("data/audio/voice/death.wav");
			voice_breath		 = SoundHandle::Load("data/audio/voice/breath.wav");
			voice_breath_run	 = SoundHandle::Load("data/audio/voice/breath_run.wav");

			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);

		}
		void Duplicate(Audios& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->shot_path = tgt.shot_path;
			this->slide_path = tgt.slide_path;
			this->trigger_path = tgt.trigger_path;
			this->shot = tgt.shot.Duplicate();
			this->explosion = tgt.explosion.Duplicate();
			this->slide = tgt.slide.Duplicate();
			this->trigger = tgt.trigger.Duplicate();
			this->magazine_down = tgt.magazine_down.Duplicate();
			this->magazine_Set = tgt.magazine_Set.Duplicate();
			this->load_ = tgt.load_.Duplicate();
			this->sort_magazine = tgt.sort_magazine.Duplicate();
			this->foot_ = tgt.foot_.Duplicate();
			this->voice_damage = tgt.voice_damage.Duplicate();
			this->voice_death = tgt.voice_death.Duplicate();
			this->voice_breath = tgt.voice_breath.Duplicate();
			this->voice_breath_run = tgt.voice_breath_run.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void Dispose(void) noexcept {
			this->shot_path = "";
			this->slide_path = "";
			this->trigger_path = "";
			this->shot.Dispose();
			this->explosion.Dispose();
			this->slide.Dispose();
			this->trigger.Dispose();
			this->magazine_down.Dispose();
			this->magazine_Set.Dispose();
			this->load_.Dispose();
			this->sort_magazine.Dispose();
			this->foot_.Dispose();
			this->voice_damage.Dispose();
			this->voice_death.Dispose();
			this->voice_breath.Dispose();
			this->voice_breath_run.Dispose();
		}
	};
	//戦車用オーディオ
	class Audios_tanks {
	private:
	public:
		SoundHandle damage;
		SoundHandle fire;
		SoundHandle reload;
		SoundHandle ricochet;
		SoundHandle engine;
		//
		void Set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			damage = SoundHandle::Load("data/audio/tank/damage/" + getparams::_str(mdata) + ".wav");
			fire = SoundHandle::Load("data/audio/tank/fire/" + getparams::_str(mdata) + ".wav");
			reload = SoundHandle::Load("data/audio/tank/reload/" + getparams::_str(mdata) + ".wav");
			ricochet = SoundHandle::Load("data/audio/tank/ricochet/" + getparams::_str(mdata) + ".wav");
			engine = SoundHandle::Load("data/audio/tank/engine.wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);
		}
		void Duplicate(const Audios_tanks& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->damage = tgt.damage.Duplicate();
			this->fire = tgt.fire.Duplicate();
			this->reload = tgt.reload.Duplicate();
			this->ricochet = tgt.ricochet.Duplicate();
			this->engine = tgt.engine.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void Dispose(void) noexcept {
			this->damage.Dispose();
			this->fire.Dispose();
			this->reload.Dispose();
			this->ricochet.Dispose();
			this->engine.Dispose();
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
		float& set_pene(void) noexcept { return pene; }
		MV1& get_model(void) noexcept { return model; }

		const auto& get_model_full(void) const noexcept { return model_full; }
		const float& get_caliber(void) const noexcept { return caliber; }
		const float& get_speed(void) const noexcept { return speed; }
		const float& get_pene(void) const noexcept { return pene; }
		const int& get_damage(void) const noexcept { return damage; }
		std::string& get_name(void) noexcept { return name; }

		auto& Set_speed(void) noexcept { return speed; }

		void Set_before(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/ammo.mv1", &this->model, true);
			MV1::Load(this->path + "/ammo2.mv1", &this->model_full, true);
		}
		void Set(void) noexcept {
			int mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_int(mdata);//ダメージ
			FileRead_close(mdata);
		}

		Ammos() {
			this->caliber = 0.f;
			this->speed = 100.f;//弾速
			this->pene = 10.f;//貫通
			this->damage = 10;//ダメージ
		}
		Ammos(const Ammos& tgt_minimam) {
			this->caliber = tgt_minimam.caliber;
			this->speed = tgt_minimam.speed;
			this->pene = tgt_minimam.pene;
			this->damage = tgt_minimam.damage;
		}
		void operator=(const Ammos& tgt_minimam) {
			this->caliber = tgt_minimam.caliber;
			this->speed = tgt_minimam.speed;
			this->pene = tgt_minimam.pene;
			this->damage = tgt_minimam.damage;
		}
	};
	//命中根
	class HIT_PASSIVE {
		//雲
		int hitss = 0;					/*hitsの数*/
		std::vector<VERTEX3D> hitsver;	/*hits*/
		std::vector<DWORD> hitsind;	    /*hits*/
		int VerBuf = -1, IndexBuf = -1;	/*hits*/
		MV1 hits;						/*hitsモデル*/
		GraphHandle hits_pic;			/*画像ハンドル*/
		int IndexNum = -1, VerNum = -1;	/*hits*/
		int vnum = -1, pnum = -1;		/*hits*/
		MV1_REF_POLYGONLIST RefMesh;	/*hits*/

		bool isUPDate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			SetUseASyncLoadFlag(FALSE);
			hits_pic = GraphHandle::Load("data/model/hit/hit.png");		 /*grass*/
			MV1::Load("data/model/hit/model.mv1", &hits, false);	//弾痕
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
		}
		//毎回のリセット
		void Clear(void) noexcept {
			hitss = 0;
			vnum = 0;
			pnum = 0;
			hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
		}

		void Set(const float &caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			hitss++;
			IndexNum = RefMesh.PolygonNum * 3 * hitss;				/*インデックスの数を取得*/
			VerNum = RefMesh.VertexNum * hitss;						/*頂点の数を取得*/
			hitsver.resize(VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.resize(IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			{
				float asize = 200.f*caliber;
				const auto& y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale)*  MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				hits.SetMatrix(mat*MATRIX_ref::Mtrans(pos));
			}
			MV1RefreshReferenceMesh(hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
			for (size_t j = 0; j < size_t(RefMesh.VertexNum); ++j) {
				auto& g = hitsver[j + vnum];
				g.pos = RefMesh.Vertexs[j].Position;
				g.norm = RefMesh.Vertexs[j].Normal;
				g.dif = RefMesh.Vertexs[j].DiffuseColor;
				g.spc = RefMesh.Vertexs[j].SpecularColor;
				g.u = RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
					hitsind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
			}
			vnum += RefMesh.VertexNum;
			pnum += RefMesh.PolygonNum * 3;
			isUPDate = true;
		}
		void update(void) noexcept {
			if (isUPDate) {
				isUPDate = false;
				VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, hitsver.data(), VerNum, VerBuf);
				SetIndexBufferData(0, hitsind.data(), IndexNum, IndexBuf);
			}
		}
		void draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			{
				DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, hits_pic.get(), TRUE);
			}
			//SetDrawAlphaTest(-1, 0);
		}
	};
	class HIT_BLOOD_PASSIVE {
		//雲
		int hitss = 0;					/*hitsの数*/
		std::vector<VERTEX3D> hitsver;	/*hits*/
		std::vector<DWORD> hitsind;	    /*hits*/
		int VerBuf = -1, IndexBuf = -1;	/*hits*/
		MV1 hits;						/*hitsモデル*/
		GraphHandle hits_pic;			/*画像ハンドル*/
		int IndexNum = -1, VerNum = -1;	/*hits*/
		int vnum = -1, pnum = -1;		/*hits*/
		MV1_REF_POLYGONLIST RefMesh;	/*hits*/

		bool isUPDate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			SetUseASyncLoadFlag(FALSE);
			hits_pic = GraphHandle::Load("data/model/hit_blood/hit.png");		 /*grass*/
			MV1::Load("data/model/hit_blood/model.mv1", &hits, false);	//弾痕
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
		}
		//毎回のリセット
		void Clear(void) noexcept {
			hitss = 0;
			vnum = 0;
			pnum = 0;
			hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
		}

		void Set(const float &caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			hitss++;
			IndexNum = RefMesh.PolygonNum * 3 * hitss;				/*インデックスの数を取得*/
			VerNum = RefMesh.VertexNum * hitss;						/*頂点の数を取得*/
			hitsver.resize(VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			hitsind.resize(IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			{
				float asize = 200.f*caliber;
				const auto& y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize, asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale)*  MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				hits.SetMatrix(mat*MATRIX_ref::Mtrans(pos));
			}
			MV1RefreshReferenceMesh(hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
			for (size_t j = 0; j < size_t(RefMesh.VertexNum); ++j) {
				auto& g = hitsver[j + vnum];
				g.pos = RefMesh.Vertexs[j].Position;
				g.norm = RefMesh.Vertexs[j].Normal;
				g.dif = RefMesh.Vertexs[j].DiffuseColor;
				g.spc = RefMesh.Vertexs[j].SpecularColor;
				g.u = RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
					hitsind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
			}
			vnum += RefMesh.VertexNum;
			pnum += RefMesh.PolygonNum * 3;
			isUPDate = true;
		}
		void update(void) noexcept {
			if (isUPDate) {
				isUPDate = false;
				VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, hitsver.data(), VerNum, VerBuf);
				SetIndexBufferData(0, hitsind.data(), IndexNum, IndexBuf);
			}
		}
		void draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			{
				DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, hits_pic.get(), TRUE);
			}
			//SetDrawAlphaTest(-1, 0);
		}
	};
	//実際に発射される弾
	class BULLETS {
	private:
		//引き継ぐ
		bool hit_Flag{ false };
		float hit_cnt{ 0.f };
		bool Flag{ false };
		bool DrawFlag{ false };
		float cnt{ 0.f };
		Ammos* spec{ nullptr };
		moves move;
	public:
		float hit_alpha{ 0.f };
		int hit_window_x{ 0 };
		int hit_window_y{ 0 };
	public:
		void Set(void) noexcept {
			this->Flag = false;
			this->DrawFlag = false;
		}
		void Put(Ammos* spec_t, const moves& move_) noexcept {
			this->hit_Flag = false;
			this->Flag = true;
			this->DrawFlag = true;
			this->cnt = 0.f;
			this->spec = spec_t;
			this->move = move_;
			this->move.repos = this->move.pos;
		}

		template<class Chara>
		bool subHP(int sel, int damage, const std::shared_ptr<Chara>&tgt, const std::shared_ptr<Chara>& mine, const std::vector<std::shared_ptr<Chara>>& chara) {
			auto q = tgt->map_col_line(this->move.repos, this->move.pos, sel);
			if (q.HitFlag == TRUE) {
				this->move.pos = q.HitPosition;
				//hit
				//mine->effcs[ef_reco].Set(this->pos, q.Normal, 0.1f / 0.1f);
				mine->Set_eff(ef_hitblood,this->move.pos, q.Normal, 0.1f / 0.1f);
				//
				this->hit_Flag = true;
				this->Flag = false;

				auto old = tgt->HP;
				tgt->HP = std::clamp(tgt->HP - damage, 0, tgt->HP_full);
				tgt->HP_parts[sel] = std::clamp(tgt->HP_parts[sel] - damage, 0, tgt->HP_full);
				tgt->got_damage = old - tgt->HP;
				tgt->got_damage_color = GetColor(255, 255, 255);

				if (float(tgt->HP) / float(tgt->HP_full) <= 0.66) {
					tgt->got_damage_color = GetColor(255, 255, 0);
				}
				if (float(tgt->HP) / float(tgt->HP_full) <= 0.33) {
					tgt->got_damage_color = GetColor(255, 128, 0);
				}
				if (damage != tgt->got_damage) {
					tgt->got_damage_color = GetColor(255, 0, 0);
				}
				tgt->got_damage_x = -255 + GetRand(255 * 2);
				tgt->got_damage_f = 1.f;
				{
					float x_1 = sinf(tgt->get_body_yrad());
					float y_1 = cosf(tgt->get_body_yrad());
					auto vc = (mine->get_pos() - tgt->get_pos()).Norm();
					tgt->got_damage_.resize(tgt->got_damage_.size() + 1);
					tgt->got_damage_.back().alpfa = 1.f;
					tgt->got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
				}
				if (!tgt->get_alive()) {
					mine->scores.set_kill(&tgt - &chara.front(), 70);
					tgt->scores.set_death(&mine - &chara.front());
					tgt->get_audio().voice_death.play_3D(tgt->get_pos(), 10.f);
				}
				else {
					tgt->get_audio().voice_damage.play_3D(tgt->get_pos(), 10.f);
				}
				return true;
			}
			return false;
		}

		template<class vehicles>
		bool get_coli(const std::shared_ptr<vehicles>&tgt,int m) {
			tgt->hitres[m] = tgt->map_col_line(this->move.repos, this->move.pos, m);
			if (tgt->hitres[m].HitFlag == TRUE) {
				tgt->hitssort[m] = { m, (this->move.repos - tgt->hitres[m].HitPosition).size() };
				return true;
			}
			else {
				tgt->hitssort[m] = { m, (std::numeric_limits<float>::max)() };
				return false;
			}
		}

		template<class Chara, class Map, class vehicles>
		void UpDate(const std::shared_ptr<Chara>& mine, const std::vector<std::shared_ptr<Chara>>& chara,
			const std::shared_ptr<vehicles>& mine_v, const std::vector<std::shared_ptr<vehicles>>& vehicle,
			HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p,std::shared_ptr<Map>& MAPPTs) noexcept {
			if (this->Flag) {
				this->move.repos = this->move.pos;
				this->move.SetPos(this->move.pos + (this->move.vec * (this->spec->get_speed() / FPS)));
				//判定
				{
					auto p = MAPPTs->map_col_line(this->move.repos, this->move.pos);
					if (p.HitFlag == TRUE) {
						this->move.pos = p.HitPosition;
					}
					//*
					bool is_hit = false;
					bool is_hitall = false;
					std::optional<size_t> hitnear;
					for (auto& tgt : vehicle) {
						if (tgt == mine_v) {
							continue;
						}
						if (tgt->set_ref_col(this->move.repos, this->move.pos)) {
							is_hitall |= is_hit;
							is_hit = false;
							//とりあえず当たったかどうか探す
							{
								for (auto& m : tgt->use_veh.Get_module_mesh()) { is_hit |= get_coli(tgt, m); }	//モジュール
								for (auto& m : tgt->use_veh.Get_space_mesh()) { is_hit |= get_coli(tgt, m); }		//空間装甲
								for (auto& m : tgt->use_veh.Get_armer_mesh()) { is_hit |= get_coli(tgt, m.first); }		//装甲
							}
							//当たってない場合抜ける
							if (!is_hit) {
								continue;
							}
							auto old = tgt->HP;
							//当たり判定を近い順にソート
							std::sort(tgt->hitssort.begin(), tgt->hitssort.end(), [](const pair_hit& x, const pair_hit& y) { return x.second < y.second; });
							//ダメージ面に届くまで判定
							for (auto& tt : tgt->hitssort) {
								//装甲面に当たらなかったならスルー
								if (tt.second == (std::numeric_limits<float>::max)()) {
									break;
								}
								//AP
								{
									//装甲面に当たったのでhitnearに代入して終了
									for (auto& a : tgt->use_veh.Get_armer_mesh()) {
										if (tt.first == a.first) {
											hitnear = tt.first;
											//ダメージ面に当たった時に装甲値に勝てるかどうか
											{
												VECTOR_ref normal = tgt->hitres[hitnear.value()].Normal;
												VECTOR_ref position = tgt->hitres[hitnear.value()].HitPosition;
												//貫通
												if (this->spec->get_pene() > a.second * (1.0f / std::abs(this->move.vec.Norm().dot(normal)))) {
													tgt->HP = std::max<int>(tgt->HP - this->spec->get_damage(), 0); //
													tgt->HP_parts[tt.first] = std::max<int>(tgt->HP_parts[tt.first] - 30, 0); //
													//撃破時エフェクト
													if (tgt->HP == 0) {
														//set_effect(&tgt->effcs[ef_bomb], tgt->obj.frame(tgt->use_veh.gunframe[0].frame1.first), VGet(0, 0, 0));
													}
													this->Flag = false;						//弾フラグ削除
													//tgt->hit_obj[tgt->hitbuf].use = 0;	//弾痕
												}
												//はじく
												else {
													//tgt->hit_obj[tgt->hitbuf].use = 1;	//弾痕
												}
												//弾処理
												this->move.vec = (this->move.vec + normal * ((this->move.vec.dot(normal)) * -2.0f)).Norm();
												this->move.pos = this->move.vec * (0.1f) + position;
												this->spec->set_pene() /= 2.0f;
												//
												if (this->spec->get_caliber() >= 0.020f) {
													mine->Set_eff(ef_reco, this->move.pos, normal);
												}
												else {
													mine->Set_eff(ef_gndsmoke, this->move.pos, normal, 0.025f / 0.1f);
												}
												//弾痕のセット
												/*
												{
													float asize = this->spec->get_caliber() * 100.f;
													auto scale = VGet(asize / std::abs(this->move.vec.Norm().dot(normal)), asize, asize);
													auto y_vec = MATRIX_ref::Vtrans(normal, tgt->move.mat.Inverse() * MATRIX_ref::RotY(deg2rad(180)));
													auto z_vec = MATRIX_ref::Vtrans(normal.cross(this->move.vec), tgt->move.mat.Inverse() * MATRIX_ref::RotY(deg2rad(180)));

													tgt->hit_obj[tgt->hitbuf].mat = MATRIX_ref::Scale(scale)* MATRIX_ref::Axis1(y_vec.cross(z_vec), y_vec, z_vec);
													tgt->hit_obj[tgt->hitbuf].move.pos = MATRIX_ref::Vtrans(position - tgt->move.pos, tgt->move.mat.Inverse() * MATRIX_ref::RotY(deg2rad(180))) + y_vec * 0.02f;
													tgt->hit_obj[tgt->hitbuf].Flag = true;
													++tgt->hitbuf %= tgt->hit_obj.size();
												}
												*/
											}
											break;
										}
									}
									if (hitnear.has_value()) {
										break;
									}
									//空間装甲、モジュールに当たったのでモジュールに30ダメ、貫徹力を1/2に
									for (auto& a : tgt->use_veh.Get_space_mesh()) {
										if (tt.first == a) {
											if (this->spec->get_caliber() >= 0.020f) {
												mine->Set_eff(ef_reco, VECTOR_ref(tgt->hitres[tt.first].HitPosition) + VECTOR_ref(tgt->hitres[tt.first].Normal) * (0.1f), tgt->hitres[tt.first].Normal);
											}
											else {
												mine->Set_eff(ef_gndsmoke, VECTOR_ref(tgt->hitres[tt.first].HitPosition) + VECTOR_ref(tgt->hitres[tt.first].Normal) * (0.1f), tgt->hitres[tt.first].Normal, 0.025f / 0.1f);
											}
											tgt->HP_parts[tt.first] = std::max<int>(tgt->HP_parts[tt.first] - 30, 0); //
											this->spec->set_pene() /= 2.0f;
										}
									}
									for (auto& a : tgt->use_veh.Get_module_mesh()) {
										if (tt.first == a) {
											if (this->spec->get_caliber() >= 0.020f) {
												mine->Set_eff(ef_reco, VECTOR_ref(tgt->hitres[tt.first].HitPosition) + VECTOR_ref(tgt->hitres[tt.first].Normal) * (0.1f), tgt->hitres[tt.first].Normal);
											}
											else {
												mine->Set_eff(ef_gndsmoke, VECTOR_ref(tgt->hitres[tt.first].HitPosition) + VECTOR_ref(tgt->hitres[tt.first].Normal) * (0.1f), tgt->hitres[tt.first].Normal, 0.025f / 0.1f);
											}
											tgt->HP_parts[tt.first] = std::max<int>(tgt->HP_parts[tt.first] - 30, 0); //
											this->spec->set_pene() /= 2.0f;
										}
									}
								}
							}
							{
								this->hit_Flag = true;
								tgt->got_damage = old - tgt->HP;
								tgt->got_damage_color = GetColor(255, 255, 255);

								if (float(tgt->HP) / float(tgt->HP_full) <= 0.66) {
									tgt->got_damage_color = GetColor(255, 255, 0);
								}
								if (float(tgt->HP) / float(tgt->HP_full) <= 0.33) {
									tgt->got_damage_color = GetColor(255, 128, 0);
								}
								//if (damage != tgt->got_damage) {
								//	tgt->got_damage_color = GetColor(255, 0, 0);
								//}
								tgt->got_damage_x = -255 + GetRand(255 * 2);
								tgt->got_damage_f = 1.f;
								{
									float x_1 = sinf(tgt->get_body_yrad());
									float y_1 = cosf(tgt->get_body_yrad());
									auto vc = (mine->get_pos() - tgt->move.pos).Norm();
									tgt->got_damage_.resize(tgt->got_damage_.size() + 1);
									tgt->got_damage_.back().alpfa = 1.f;
									tgt->got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
								}
								/*
								if (!tgt->get_alive()) {
									mine->scores.set_kill(&tgt - &chara.front(), 70);
									tgt->scores.set_death(&mine - &chara.front());
									tgt->get_audio().voice_death.play_3D(tgt->get_pos(), 10.f);
								}
								else {
									tgt->get_audio().voice_damage.play_3D(tgt->get_pos(), 10.f);
								}
								*/
							}
							if (hitnear.has_value()) {
								break;
							}
						}
					}
					for (auto& tgt : chara) {
						if (tgt == mine || !tgt->get_alive()) {
							continue;
						}
						if (tgt->set_ref_col(this->move.repos, this->move.pos)) {
							//HEAD
							if (subHP(0, this->spec->get_damage() * 3, tgt, mine, chara)) {
								break;
							}
							//BODY
							if (subHP(1, this->spec->get_damage(), tgt, mine, chara)) {
								break;
							}
							//LEFTHAND
							if (subHP(2, this->spec->get_damage() * 2 / 5, tgt, mine, chara)) {
								break;
							}
							//RIGHTHAND
							if (subHP(3, this->spec->get_damage() * 2 / 5, tgt, mine, chara)) {
								break;
							}
							//LEG
							if (subHP(4, this->spec->get_damage() * 4 / 5, tgt, mine, chara)) {
								break;
							}
						}
					}
					if (p.HitFlag == TRUE && !is_hitall) {
						if (this->Flag) {
							this->Flag = false;
							//弾痕
							mine->Set_gndhit(this->move.pos, p.Normal, 0.025f / 0.1f);
							hit_obj_p.Set(this->spec->get_caliber(), this->move.pos, p.Normal, this->move.vec);
							return;
						}
					}
					//血痕
					if (!this->Flag && !is_hitall) {
						if (!p.HitFlag == TRUE) {
							auto vec = (this->move.pos - this->move.repos).Norm();
							p = MAPPTs->map_col_line(this->move.pos, this->move.pos + vec * 5.f);
						}
						if (p.HitFlag == TRUE) {
							mine->Set_gndhit(p.HitPosition, p.Normal, 0.025f / 0.1f);
							hit_b_obj_p.Set(0.35f, p.HitPosition, p.Normal, this->move.vec);
						}
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
		void Set_Draw(void) noexcept {
			if (this->hit_Flag) {
				this->hit_Flag = false;
				this->hit_cnt = 0.25f;
				auto p = ConvWorldPosToScreenPos(this->move.pos.get());
				if (p.z >= 0.f&&p.z <= 1.f) {
					this->hit_alpha = 1.f;
					this->hit_window_x = int(p.x);
					this->hit_window_y = int(p.y);
				}
			}
			if (this->hit_cnt > 0.f) {
				easing_set(&this->hit_alpha, 2.f, 0.95f);
				this->hit_cnt -= 1.f / FPS;
			}
			else {
				easing_set(&this->hit_alpha, 0.f, 0.8f);
				this->hit_cnt = 0.f;
			}
		}
		void Draw(void) noexcept {
			if (this->DrawFlag) {
				DXDraw::Capsule3D(this->move.pos, this->move.repos, ((this->spec->get_caliber() - 0.00762f) * 0.1f + 0.00762f), GetColor(255, 255, 172), GetColor(255, 255, 255));
				if (!this->Flag) {
					this->DrawFlag = false;
				}
			}
		}
	};
	//パフォーマンス
	class performance {
	public:
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
	//
	class BASE_Obj {
	private:
	public:
		size_t id_t = 0;
		Models mod;
		void Set(size_t id_) {
			this->id_t = id_;
		}
	};

	//パーツデータ
	class GUNPARTs :public BASE_Obj {
		int type = 0;
	public:
		float recoil_xup = 0.f;				//反動
		float recoil_xdn = 0.f;				//反動
		float recoil_yup = 0.f;				//反動
		float recoil_ydn = 0.f;				//反動
		float reload_time = 1.f;			//再装填時間
		std::vector <uint8_t> select;		//セレクター
		std::vector <EnumSELECT_LAM> select_lam;	//ライト・レーザー
		performance per;					//common
		size_t mazzule_type = 0;			//mazzule
		GraphHandle reticle;				//sight
		float zoom = 1.f;					//sight
		float reticle_size = 600.f;			//sight
		float zoom_size = 600.f;			//sight
		size_t stock_type = 0;				//stock
		std::vector<std::string> can_attach;
		std::vector<Ammos> ammo;
		Audios audio;
		//magazine
		size_t mag_cnt = 1;
	public:
		//setter
		void Set_type(int type_t) { this->type = type_t; }
		//
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&](void) noexcept {
				//共通データ
				{
					per.name = getparams::get_str(this->mod.mdata);		//名前
					per.info = getparams::get_str(this->mod.mdata);		//説明
				}
				//
				if (this->type == PARTS_MAZZULE) {
					mazzule_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == PARTS_STOCK) {
					stock_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == PARTS_BASE) {
					//セレクター設定
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "semi") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_SEMI));	//セミオート=1
						}
						else if (getparams::getright(p.c_str()) == "full") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_FULL));	//フルオート=2
						}
						else if (getparams::getright(p.c_str()) == "3b") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_3B));	//3連バースト=3
						}
						else if (getparams::getright(p.c_str()) == "2b") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_2B));	//2連バースト=4
						}
						else {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_SEMI));
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
				if (this->type == PARTS_MAGAZINE) {
					this->mag_cnt = getparams::_long(this->mod.mdata);			//弾数
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				if (this->type == PARTS_LAM) {
					//レーザーかライトか
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "laser") {
							this->select_lam.emplace_back(SELECTLAM_LASER);	//レーザー
						}
						else if (getparams::getright(p.c_str()) == "light") {
							this->select_lam.emplace_back(SELECTLAM_LIGHT);	//ライト
						}
						else {
							this->select_lam.emplace_back(SELECTLAM_LASER);	//レーザー
						}
					}
				}
				if (this->type == PARTS_SIGHT) {
					this->zoom = getparams::_float(this->mod.mdata);
					this->reticle_size = getparams::_float(this->mod.mdata);
					this->zoom_size = getparams::_float(this->mod.mdata);
				}
				//共通データ
				{
					per.recoil = getparams::_float(this->mod.mdata);		//リコイル
					per.weight = getparams::_float(this->mod.mdata);		//重量
					{
						can_attach.clear();
						while (FileRead_eof(this->mod.mdata) == 0) {
							can_attach.emplace_back(getparams::get_str(this->mod.mdata));
						}
					}
				}
			});
			if (this->type == PARTS_SIGHT) {
				SetUseASyncLoadFlag(FALSE);
				this->reticle = GraphHandle::Load(this->mod.get_path() + "/reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
			if (this->type == PARTS_BASE) {
				//フレーム
				for (auto& a : this->ammo) {
					a.Set();
				}
			}
		}
		//
		void Set_Ammos_data(int mdata) {
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
		uint8_t Select_Chose(uint8_t sel_chose) {
			auto ans = std::find(this->select.begin(), this->select.end(), sel_chose);
			return uint8_t((ans != this->select.end()) ? (ans - this->select.begin()) : -1);
		}
		void Set_gun_select(std::vector<MV1::ani*>&gunanime_sel, int selecting) {
			for (auto& sel : this->select) {
				easing_set(&gunanime_sel[&sel - &this->select[0]]->per, float(int(sel == this->select[selecting])), 0.5f);
			}
		}
	};
	//薬品データ
	class Meds :public BASE_Obj {
	public:
		int repair = 0;
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&](void) noexcept {
				this->repair = getparams::_long(this->mod.mdata);//
			});
		}
	};
	//薬品データ
	class Grenades :public BASE_Obj {
	public:
		float time = 0;
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&](void) noexcept {
				this->time = getparams::_float(this->mod.mdata);//
			});
		}
	};

	//アイテム
	class Items {
	private:
		//共通
		moves move;
		MV1 obj;
		float del_timer = 0.f;
		size_t id_t = 0;
		//マガジン専用パラメーター
		GUNPARTs* ptr_mag{ nullptr };
		GUNPARTs magazine_param;
		//治療キット専用パラメーター
		Meds* ptr_med{ nullptr };
		Meds medkit;
		//治療キット専用パラメーター
		Grenades* ptr_gre{ nullptr };
		Grenades grenades;
	public:
		bool flag_canlook_player{ true };
		auto& get_ptr_mag(void) const noexcept { return ptr_mag; }
		auto& get_ptr_med(void) const noexcept { return ptr_med; }
		auto& get_magazine(void) const noexcept { return magazine_param; }
		auto& get_pos_(void) const noexcept { return move.pos; }
	private:
		//mag
		void Set_item(GUNPARTs*magdata, const moves& move_, size_t dnm = SIZE_MAX) {
			this->move = move_;
			this->ptr_mag = magdata;
			this->obj = this->ptr_mag->mod.get_model().Duplicate();
			if (dnm == SIZE_MAX) {
				if (this->ptr_mag != nullptr) {
					this->magazine_param.mag_cnt = int(this->ptr_mag->mag_cnt);
				}
			}
			else {
				this->magazine_param.mag_cnt = dnm;
			}
			if (this->ptr_mag != nullptr) {
				if (this->magazine_param.ammo.size() < this->ptr_mag->ammo.size()) {
					this->magazine_param.ammo.resize(this->ptr_mag->ammo.size());
				}
				this->magazine_param.ammo[0].get_name() = this->ptr_mag->ammo[0].get_name();
			}
			this->del_timer = (this->magazine_param.mag_cnt == 0) ? 5.f : 20.f;
		}
		//med
		void Set_item(Meds*meddata, const moves& move_) {
			this->move = move_;
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.get_model().Duplicate();
		}
		//med
		void Set_item(Grenades*gredata, const moves& move_) {
			this->move = move_;
			this->ptr_gre = gredata;
			this->obj = this->ptr_gre->mod.get_model().Duplicate();
			this->del_timer = this->ptr_gre->time;
		}
	public:
		//mag
		Items(size_t id, GUNPARTs*magdata, const moves& move_, size_t dnm = SIZE_MAX) {
			this->id_t = id;
			Set_item(magdata, move_, dnm);
		}
		bool Set_item_(GUNPARTs*magdata, const moves& move_, size_t dnm) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(magdata, move_, dnm);
				return true;
			}
			return false;
		}
		//med
		Items(size_t id, Meds*meddata, const moves& move_) {
			this->id_t = id;
			this->Set_item(meddata, move_);
		}
		bool Set_item_(Meds*meddata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(meddata, move_);
				return true;
			}
			return false;
		}
		//gre
		Items(size_t id, Grenades*gredata, const moves& move_) {
			this->id_t = id;
			this->Set_item(gredata, move_);
		}
		bool Set_item_(Grenades*gredata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(gredata, move_);
				return true;
			}
			return false;
		}
		//
		template<class Y>
		void UpDate(std::vector<std::shared_ptr<Items>>& item, std::shared_ptr<Y>& MAPPTs) {
			auto old = this->move.pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
				this->obj.SetMatrix(this->move.mat*MATRIX_ref::Mtrans(this->move.pos));
				this->move.Update_Physics();
				for (auto& p : item) {
					if ((p->ptr_mag != nullptr || p->ptr_med != nullptr || p->ptr_gre != nullptr) && p.get() != this) {
						if ((p->move.pos - this->move.pos).size() <= 0.35f) {
							p->move.vec.xadd((p->move.pos - this->move.pos).x()*5.f / FPS);
							p->move.vec.zadd((p->move.pos - this->move.pos).z()*5.f / FPS);
							this->move.vec.xadd((this->move.pos - p->move.pos).x()*5.f / FPS);
							this->move.vec.zadd((this->move.pos - p->move.pos).z()*5.f / FPS);
						}
					}
				}
				if (this->ptr_gre != nullptr) {
					auto pp = MAPPTs->map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (pp.HitFlag == TRUE) {
						this->move.HitGround(pp, 0.005f);
						auto fvec = this->move.vec.Norm();
						auto nvec = VECTOR_ref(pp.Normal).Norm();
						this->move.vec = (fvec + nvec * ((fvec*-1.f).dot(nvec)*2.f))*(this->move.vec.size()*0.5f);
					}
				}
				else {
					auto pp = MAPPTs->map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (pp.HitFlag == TRUE) {
						this->move.HitGround(pp, 0.005f);
						this->move.vec.clear();
						//easing_set(&this->move.vec, VECTOR_ref::vget(0, 0, 0), 0.8f);
					}
				}
				//
			}
			//
			if (this->ptr_mag != nullptr || this->ptr_gre != nullptr) {
				this->del_timer -= 1.f / FPS;
			}
		}
		template<class Y, class Chara>
		void Get_item_2(VECTOR_ref StartPos, VECTOR_ref EndPos, const std::shared_ptr<Chara>& chara, std::shared_ptr<Y>& MAPPTs) {
			if (this->flag_canlook_player) {
				bool zz = false;
				if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
					auto p = MAPPTs->map_col_line(StartPos, EndPos);
					if (p.HitFlag == TRUE) {
						EndPos = p.HitPosition;
					}
					zz = (Segment_Point_MinLength(StartPos.get(), EndPos.get(), this->move.pos.get()) <= 0.4f);
				}
				//個別
				if (this->ptr_mag != nullptr) {
					chara->addf_canget_magitem(zz);
					if (zz) {
						chara->set_canget_mag(this->id_t, this->ptr_mag->mod.get_name());
						if (chara->getmagazine_push() && this->magazine_param.mag_cnt != 0 && (this->ptr_mag->ammo[0].get_name() == this->magazine_param.ammo[0].get_name())) {
							chara->sort_f = false;
							chara->gun_stat_now->magazine_plus(this);
							if (chara->gun_stat_now->get_magazine_in().size() == 1) {
								chara->reloadf = true;
							}
							this->Detach_item();
						}
					}
				}
				//
				if (this->ptr_med != nullptr) {
					chara->addf_canget_meditem(zz);
					if (zz) {
						chara->set_canget_med(this->id_t, this->ptr_med->mod.get_name());
						if (chara->getmagazine_push()) {
							chara->HP = std::clamp<int>(chara->HP + this->ptr_med->repair, 0, chara->HP_full);
							this->Detach_item();
						}
					}
				}
				//
				/*
				if (this->ptr_gre != nullptr) {
					chara->addf_canget_meditem(zz);
					if (zz) {
						chara->set_canget_med(this->id_t, this->ptr_gre->mod.get_name());
						if (chara->getmagazine_push()) {
							chara->HP = std::clamp<int>(chara->HP + this->ptr_gre->repair, 0, chara->HP_full);
							this->Detach_item();
						}
					}
				}
				*/
				//
			}
		}
		template<class Y>
		void Check_CameraViewClip(std::shared_ptr<Y>& MAPPTs, bool use_occlusion) noexcept {
			this->flag_canlook_player = true;
			auto ttt = this->move.pos;
			if (CheckCameraViewClip_Box((ttt + VECTOR_ref::vget(-0.3f, 0, -0.3f)).get(), (ttt + VECTOR_ref::vget(0.3f, 0.3f, 0.3f)).get())) {
				this->flag_canlook_player = false;
				return;
			}
			if (use_occlusion) {
				if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.3f, 0)).HitFlag == TRUE &&
					MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.0f, 0)).HitFlag == TRUE) {
					this->flag_canlook_player = false;
					return;
				}
			}
		}

		void Draw_item(void) noexcept {
			if (this->flag_canlook_player) {
				if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
					this->obj.DrawModel();
				}
			}
		}
		void Detach_item(void) noexcept {
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->ptr_gre = nullptr;
			this->obj.Dispose();
		}
		bool Detach_mag(void) noexcept {
			if (this->ptr_mag != nullptr && this->del_timer <= 0.f) {
				this->Detach_item();
				return true;
			}
			return false;
		}
		template<class Y, class Chara>
		bool Detach_gre(std::shared_ptr<Y>& MAPPTs, std::shared_ptr<Chara>& mine, std::vector<std::shared_ptr<Chara>>& chara) noexcept {
			if (this->ptr_gre != nullptr && this->del_timer <= 0.f) {
				//effect
				mine->calc_gre_effect(this->move.pos);
				//
				mine->get_audio().explosion.vol(255);
				mine->get_audio().explosion.play_3D(this->move.pos, 100.f);
				//グレ爆破
				this->Detach_item();
				for (auto& tgt : chara) {
					tgt->calc_gredamage(mine, chara);
					if (tgt->get_alive()) {
						float scale = (this->move.pos - tgt->get_head_pos()).size();
						if (scale < 10.f) {
							if (!MAPPTs->map_col_line(this->move.pos, tgt->get_head_pos()).HitFlag == TRUE) {
								int damage = int(150.f * (10.f - scale) / 10.f);
								damage = std::clamp(damage, 0, 100);
								auto old = tgt->HP;
								tgt->HP = std::clamp(tgt->HP - damage, 0, tgt->HP_full);
								tgt->HP_parts[0] = std::clamp(tgt->HP_parts[0] - damage, 0, tgt->HP_full);
								tgt->got_damage = old - tgt->HP;
								tgt->got_damage_color = GetColor(255, 255, 255);

								if (float(tgt->HP) / float(tgt->HP_full) <= 0.66) {
									tgt->got_damage_color = GetColor(255, 255, 0);
								}
								if (float(tgt->HP) / float(tgt->HP_full) <= 0.33) {
									tgt->got_damage_color = GetColor(255, 128, 0);
								}
								if ((damage) != tgt->got_damage) {
									tgt->got_damage_color = GetColor(255, 0, 0);
								}
								tgt->got_damage_x = -255 + GetRand(255 * 2);
								tgt->got_damage_f = 1.f;
								{
									float x_1 = sinf(tgt->get_body_yrad());
									float y_1 = cosf(tgt->get_body_yrad());
									auto vc = (mine->get_pos() - tgt->get_pos()).Norm();
									tgt->got_damage_.resize(tgt->got_damage_.size() + 1);
									tgt->got_damage_.back().alpfa = 1.f;
									tgt->got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
								}
								if (!tgt->get_alive()) {
									mine->scores.set_kill(&tgt - &chara.front(), 70);
									tgt->scores.set_death(&mine - &chara.front());
									tgt->get_audio().voice_death.play_3D(tgt->get_pos(), 10.f);
								}
								else {
									tgt->get_audio().voice_damage.play_3D(tgt->get_pos(), 10.f);
								}
								tgt->gre_eff = true;
								break;
							}
						}
					}
				}
				return true;
			}
			return false;
		}

		// コピーは禁止するが、ムーブは許可する
		Items(const Items&) = delete;
		Items& operator=(const Items&) = delete;

		// 特殊メンバ関数を明示的に定義もしくはdeleteした場合、
		// それ以外の特殊メンバ関数は明示的に定義もしくはdefault宣言しなければ
		// 暗黙定義されない
		Items(Items&&) = default;
		Items() = default;
		Items& operator=(Items&&) = default;

		~Items() {}
	};

	//todo:親クラス作る
	class b2_frame {
	public:
		//変更不可
		frames frame;
	};
	class foot_frame:public b2_frame {
	public:
		float will_y = 0.f;
	};
	class cat_frame :public b2_frame {
	public:
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
	//戦車砲
	class gun_frame {
		int type = 0;
	public:
		float xrad = 0.f, yrad = 0.f;
		uint16_t rounds = 0;
		std::string name;
		float load_time = 0.f;
		std::vector<std::string> useammo;
		std::vector<Mainclass::Ammos> Spec;	/**/
		frames frame1;
		frames frame2;
		frames frame3;
	public:
		void set(const MV1& obj, int i) {
			this->frame1 = { i,obj.frame(i) };
			auto p2 = obj.frame_parent(this->frame1.first);
			if (p2 >= 0) {
				this->frame1.second -= obj.frame(int(p2)); //親がいる時引いとく
			}
			if (obj.frame_child_num(this->frame1.first) > 0) {
				if (obj.frame_name(this->frame1.first + 1).find("仰角", 0) != std::string::npos) {
					this->frame2 = { this->frame1.first + 1,obj.frame(this->frame1.first + 1) - obj.frame(this->frame1.first) };
					if (obj.frame_child_num(this->frame1.first) > 0) {
						this->frame3 = { this->frame2.first + 1,obj.frame(this->frame2.first + 1) - obj.frame(this->frame2.first) };
					}
					else {
						this->frame3.first = -1;
					}
				}
			}
			else {
				this->frame2.first = -1;
			}
		}

		void Set_Ammos_data(int mdata) {
			this->Spec.resize(this->Spec.size() + 1);
			this->Spec.back().Set_before("data/ammo/", getparams::_str(mdata));

		}

		void Set_Ammos_after() {
			for (auto& a : this->Spec) {
				a.Set();
			}
		}

	};
	class Guns {							/**/
		size_t use_bullet{ 0 };							/*確保する弾*/
		std::array<BULLETS, max_bullet> bullet;			/*確保する弾*/

		float loadcnt{ 0 };					/*装てんカウンター*/
		float fired{ 0.f };					/*駐退数*/
		int16_t rounds{ 0 };				/*弾数*/
		gun_frame gun_info;					/**/
		std::vector<Mainclass::Ammos> Spec;	/**/
	public:
		const auto& Getfired() { return fired; }
		const auto& Getgun_info() { return gun_info; }
		const auto& Getcaliber(size_t id) { return Spec[id].get_caliber(); }

		void Clear() {
			this->fired = 0.f;
			this->loadcnt = 0.f;
			this->rounds = 0;
			this->use_bullet = 0;
			this->Spec.clear();
		}
		void Set(const gun_frame& resorce) {
			this->gun_info = resorce;
			this->rounds = this->gun_info.rounds;
			//使用砲弾
			this->Spec = this->gun_info.Spec;
			for (auto& a : this->bullet) { a.Set(); }			//弾
		}
		void SetGunRad(const float view_xrad, const float view_yrad, float limit) {
			this->gun_info.yrad += std::clamp(view_yrad, -limit, limit);
			//this->gun_info.yrad = std::clamp(this->gun_info.yrad + std::clamp(view_yrad, -limit, limit),deg2rad(-30.0)+yrad,deg2rad(30.0)+yrad);//射界制限
			this->gun_info.xrad = std::clamp(this->gun_info.xrad + std::clamp(view_xrad, -limit, limit), deg2rad(-10), deg2rad(20));
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
				this->loadcnt = this->Getgun_info().load_time;
				this->rounds = std::max<uint16_t>(this->rounds - 1, 0);
				this->fired = 1.f;
				mine->Set_eff(ef_fire, pos_t, vec_t, 0.1f / 0.1f);//ノーマル

				mine_v->get_audio().fire.vol(128);
				mine_v->get_audio().fire.play_3D(mine_v->move.pos, 250.f);
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
	//
	class b2Pats {
		b2FixtureDef fixtureDef;			/*動的ボディフィクスチャを定義します*/
		std::unique_ptr<b2Body> body;		/**/
		b2Fixture* playerfix{ nullptr };	/**/
	public:
		VECTOR_ref pos;/*仮*/

		void Set(b2Body* body_ptr, b2Shape* dynamicBox) {
			fixtureDef.shape = dynamicBox;								/**/
			fixtureDef.density = 1.0f;									/*ボックス密度をゼロ以外に設定すると、動的になる*/
			fixtureDef.friction = 0.3f;									/*デフォルトの摩擦をオーバーライド*/
			this->body.reset(body_ptr);									/**/
			this->playerfix = this->body->CreateFixture(&fixtureDef);	/*シェイプをボディに追加*/
		}

		void SetLinearVelocity(const b2Vec2& position) {
			this->body->SetLinearVelocity(position);
		}

		void UpDate(const VECTOR_ref& add, float yradadd) {
			this->body->SetLinearVelocity(b2Vec2(add.x(), add.z()));
			this->body->SetAngularVelocity(yradadd);
		}


		void Dispose() {
			if (this->playerfix != nullptr) {
				delete this->playerfix->GetUserData();
				this->playerfix->SetUserData(nullptr);
			}
		}

		const auto Get() { return body.get(); }
		const auto Pos() { return body->GetPosition(); }
		const auto Rad() { return body->GetAngle(); }
		const float Speed() { return std::hypot(this->body->GetLinearVelocity().x, this->body->GetLinearVelocity().y); }

		void SetTransform(const b2Vec2& position, float32 angle) {
			body->SetTransform(position, angle);
		}
	};
	//履帯BOX2D
	class FootWorld {
	public:
		std::shared_ptr<b2World> world{ nullptr };		/*足world*/
		b2RevoluteJointDef f_jointDef;					/*ジョイント*/
		std::vector<b2Pats> Foot, Wheel, Yudo;			/**/
	};
	//戦車データ
	class Vehcs {
		//共通
		std::string name;								/**/
		MV1 obj, col;									/**/
		VECTOR_ref minpos, maxpos;						/**/
		std::vector<gun_frame> gunframe;				/**/
		std::vector<foot_frame> wheelframe;				/**/
		std::vector<foot_frame> wheelframe_nospring;	/*誘導輪回転*/
		uint16_t HP = 0;								/**/
		std::vector<std::pair<int, float>> armer_mesh;	/*装甲ID*/
		std::vector<int> space_mesh;					/*装甲ID*/
		std::vector<int> module_mesh;					/*装甲ID*/
		bool isfloat = false;							/*浮くかどうか*/
		float down_in_water = 0.f;						/*沈む判定箇所*/
		float flont_speed_limit = 0.f;					/*前進速度(km/h)*/
		float back_speed_limit = 0.f;					/*後退速度(km/h)*/
		float body_rad_limit = 0.f;						/*旋回速度(度/秒)*/
		float turret_rad_limit = 0.f;					/*砲塔駆動速度(度/秒)*/
		frames fps_view;								//コックピット
		GraphHandle ui_pic;								//シルエット
		int pic_x, pic_y;								//サイズ
		std::array<int, 4> square{ 0 };//車輛の四辺
		std::array<std::vector<frames>, 2> b2upsideframe; /*履帯上*/
		std::array<std::vector<cat_frame>, 2> b2downsideframe; /*履帯*/


		void copy(const Vehcs& t) {
			this->wheelframe.clear();
			for (auto& p : t.wheelframe) {
				this->wheelframe.resize(this->wheelframe.size() + 1);
				this->wheelframe.back().frame = p.frame;
			}
			this->wheelframe_nospring.clear();
			for (auto& p : t.wheelframe_nospring) {
				this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
				this->wheelframe_nospring.back().frame = p.frame;
			}
			this->name = t.name;
			this->minpos = t.minpos;
			this->maxpos = t.maxpos;
			this->gunframe = t.gunframe;
			this->HP = t.HP;
			this->armer_mesh = t.armer_mesh;
			this->space_mesh = t.space_mesh;
			this->module_mesh = t.module_mesh;
			this->isfloat = t.isfloat;
			this->down_in_water = t.down_in_water;
			this->flont_speed_limit = t.flont_speed_limit;
			this->back_speed_limit = t.back_speed_limit;
			this->body_rad_limit = t.body_rad_limit;
			this->turret_rad_limit = t.turret_rad_limit;
			this->square = t.square;
			this->b2upsideframe = t.b2upsideframe;
			this->b2downsideframe = t.b2downsideframe;
			this->fps_view = t.fps_view;

			this->ui_pic = t.ui_pic.Duplicate();
			this->pic_x = t.pic_x;
			this->pic_y = t.pic_y;
		}
	public:
		Audios_tanks audio;
		//
		auto& Get_name() { return name; }
		const auto& Get_obj()const { return obj; }
		const auto& Get_col()const { return col; }
		auto& Get_minpos() { return minpos; }
		auto& Get_maxpos() { return maxpos; }
		auto& Get_gunframe() { return gunframe; }
		auto& Get_wheelframe() { return wheelframe; }
		auto& Get_wheelframe_nospring() { return wheelframe_nospring; }
		auto& Get_HP() { return HP; }
		auto& Get_armer_mesh() { return armer_mesh; }
		auto& Get_space_mesh() { return space_mesh; }
		auto& Get_module_mesh() { return module_mesh; }
		auto& Get_isfloat() { return isfloat; }
		auto& Get_down_in_water() { return down_in_water; }
		auto& Get_flont_speed_limit() { return flont_speed_limit; }
		auto& Get_back_speed_limit() { return back_speed_limit; }
		auto& Get_body_rad_limit() { return body_rad_limit; }
		auto& Get_turret_rad_limit() { return turret_rad_limit; }
		auto& Get_fps_view() { return fps_view; }
		auto& Get_ui_pic() { return ui_pic; }
		auto& Get_pic_x() { return pic_x; }
		auto& Get_pic_y() { return pic_y; }
		auto& Get_square() { return square; }
		auto& Get_b2upsideframe() { return b2upsideframe; }
		auto& Get_b2downsideframe() { return b2downsideframe; }
		//コンストラクタ
		Vehcs() { }
		Vehcs(const Vehcs& t) {
			this->copy(t);
		}
		void operator=(const Vehcs& t) {
			this->copy(t);
		}
		//事前読み込み
		static void set_vehicles_pre(const char* name, std::vector<Mainclass::Vehcs>* veh_, const bool& Async) {
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((std::string(name) + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						veh_->resize(veh_->size() + 1);
						veh_->back().name = win32fdt.cFileName;
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
			for (auto& t : *veh_) {
				MV1::Load(std::string(name) + t.name + "/model.mv1", &t.obj, Async);
				MV1::Load(std::string(name) + t.name + "/col.mv1", &t.col, Async);
				t.ui_pic = GraphHandle::Load(std::string(name) + t.name + "/pic.png");
			}
		}
		//メイン読み込み
		void Set_after() {
			using namespace std::literals;

			//αテスト
			this->obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			GetGraphSize(this->ui_pic.get(), &this->pic_x, &this->pic_y);
			//固有
			this->down_in_water = 0.f;
			for (int i = 0; i < this->obj.mesh_num(); i++) {
				auto p = this->obj.mesh_maxpos(i).y();
				if (this->down_in_water < p) {
					this->down_in_water = p;
				}
			}
			this->down_in_water /= 2.f;
			for (int i = 0; i < this->obj.frame_num(); i++) {
				std::string p = this->obj.frame_name(i);
				if (p.find("転輪", 0) != std::string::npos) {
					this->wheelframe.resize(this->wheelframe.size() + 1);
					this->wheelframe.back().frame = { i,this->obj.frame(i) };
				}
				else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
					this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
					this->wheelframe_nospring.back().frame = { i,this->obj.frame(i) };
				}
				else if (p.find("旋回", 0) != std::string::npos) {
					this->gunframe.resize(this->gunframe.size() + 1);
					this->gunframe.back().set(this->obj, i);
				}
				else if (p == "min") {
					this->minpos = this->obj.frame(i);
				}
				else if (p == "max") {
					this->maxpos = this->obj.frame(i);
				}
				else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
					this->b2upsideframe[0].clear();
					this->b2upsideframe[1].clear();
					for (int z = 0; z < this->obj.frame_child_num(i); z++) {
						if (this->obj.frame(i + 1 + z).x() > 0) {
							this->b2upsideframe[0].resize(this->b2upsideframe[0].size() + 1);
							this->b2upsideframe[0].back() = { i + 1 + z, this->obj.frame(i + 1 + z) };
						}
						else {
							this->b2upsideframe[1].resize(this->b2upsideframe[1].size() + 1);
							this->b2upsideframe[1].back() = { i + 1 + z, this->obj.frame(i + 1 + z) };
						}
					}
					std::sort(this->b2upsideframe[0].begin(), this->b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
					std::sort(this->b2upsideframe[1].begin(), this->b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
				}
				else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
					this->b2downsideframe[0].clear();
					this->b2downsideframe[1].clear();
					for (int z = 0; z < this->obj.frame_child_num(i); z++) {
						if (this->obj.frame(i + 1 + z).x() > 0) {
							this->b2downsideframe[0].resize(this->b2downsideframe[0].size() + 1);
							this->b2downsideframe[0].back().frame = { i + 1 + z, this->obj.frame(i + 1 + z) };
						}
						else {
							this->b2downsideframe[1].resize(this->b2downsideframe[1].size() + 1);
							this->b2downsideframe[1].back().frame = { i + 1 + z, this->obj.frame(i + 1 + z) };
						}
					}
				}

			}
			//2	左後部0
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (f.frame.second.x() >= 0) {
						this->square[0] = f.frame.first;
						tmp = f.frame.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[0] != f.frame.first) {
						if (f.frame.second.x() >= 0) {
							if (tmp < f.frame.second.z()) {
								this->square[0] = f.frame.first;
								tmp = f.frame.second.z();
							}
						}
					}
				}
			}
			//10	左前部1
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (f.frame.second.x() >= 0) {
						this->square[1] = f.frame.first;
						tmp = f.frame.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[1] != f.frame.first) {
						if (f.frame.second.x() >= 0) {
							if (tmp > f.frame.second.z()) {
								this->square[1] = f.frame.first;
								tmp = f.frame.second.z();
							}
						}
					}
				}
			}
			//3	右後部2
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (!(f.frame.second.x() >= 0)) {
						this->square[2] = f.frame.first;
						tmp = f.frame.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[2] != f.frame.first) {
						if (!(f.frame.second.x() >= 0)) {
							if (tmp < f.frame.second.z()) {
								this->square[2] = f.frame.first;
								tmp = f.frame.second.z();
							}
						}
					}
				}
			}
			//11	右前部3
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (!(f.frame.second.x() >= 0)) {
						this->square[3] = f.frame.first;
						tmp = f.frame.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[3] != f.frame.first) {
						if (!(f.frame.second.x() >= 0)) {
							if (tmp > f.frame.second.z()) {
								this->square[3] = f.frame.first;
								tmp = f.frame.second.z();
							}
						}
					}
				}
			}
			//装甲
			for (int i = 0; i < this->col.mesh_num(); i++) {
				std::string p = this->col.material_name(i);
				if (p.find("armer", 0) != std::string::npos) {
					this->armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
				}
				else if (p.find("space", 0) != std::string::npos) {
					this->space_mesh.emplace_back(i);//空間装甲
				}
				else {
					this->module_mesh.emplace_back(i);//モジュール
				}
			}
			//data
			{
				int mdata = FileRead_open(("data/tank/" + this->name + "/data.txt").c_str(), FALSE);
				this->isfloat = getparams::_bool(mdata);
				this->flont_speed_limit = getparams::_float(mdata);
				this->back_speed_limit = getparams::_float(mdata);
				this->body_rad_limit = getparams::_float(mdata);
				this->turret_rad_limit = getparams::_float(mdata);
				this->HP = uint16_t(getparams::_ulong(mdata));
				auto stt = getparams::get_str(mdata);
				for (auto& g : this->gunframe) {
					g.name = getparams::getright(stt);
					g.load_time = getparams::_float(mdata);
					g.rounds = uint16_t(getparams::_ulong(mdata));

					g.Set_Ammos_data(mdata);
					while (true) {
						auto stp = getparams::get_str(mdata);
						if (stp.find("useammo" + std::to_string(g.useammo.size())) == std::string::npos) {
							break;
						}
						g.useammo.emplace_back(getparams::getright(stp));
					}
				}
				this->audio.Set(mdata);						//サウンド
				FileRead_close(mdata);
				for (auto& g : this->gunframe) {
					g.Set_Ammos_after();
				}
			}
		}
	};
};