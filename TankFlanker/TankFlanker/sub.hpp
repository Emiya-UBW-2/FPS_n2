#pragma once
#define FRAME_RATE 90.f
//���T�C�Y
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)
//
static const size_t max_bullet{ 32 };
//
enum Effect {
	ef_fire,	//���C��
	ef_reco,	//�����a���e
	ef_smoke,	//�e�̋O��
	ef_gndsmoke,//�n�ʂ̋O��
	ef_fire2,	//���C��
	ef_hitblood,//�����Ԃ�
	ef_greexp,	//�O������
	effects,	//�ǂݍ���
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
//�L�[�o�C���h
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
				//�L�[
			case 0:
				return CheckHitKey(this->first) != 0;
			case 1:
				on_off.GetInput(CheckHitKey(this->first) != 0);
				return on_off.on();
			case 2:
				on_off.GetInput(CheckHitKey(this->first) != 0);
				return on_off.trigger();
				//�}�E�X
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
			tmp_k.second = "�O�i";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = KEY_INPUT_S;
			tmp_k.second = "���";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = KEY_INPUT_D;
			tmp_k.second = "�E����";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//2
			tmp_k.first = KEY_INPUT_A;
			tmp_k.second = "������";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//3
			tmp_k.first = KEY_INPUT_Q;
			tmp_k.second = "�����[��";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//4
			tmp_k.first = KEY_INPUT_E;
			tmp_k.second = "�E���[��";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//5
			tmp_k.first = KEY_INPUT_R;
			tmp_k.second = "�����[�h";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//6
			tmp_k.first = KEY_INPUT_F;
			tmp_k.second = "�A�C�e���擾";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//7
			tmp_k.first = KEY_INPUT_G;
			tmp_k.second = "�O���l�[�h����";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//8
			tmp_k.first = KEY_INPUT_C;
			tmp_k.second = "���Ⴊ��";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//9
			tmp_k.first = KEY_INPUT_O;
			tmp_k.second = "�^�C�g����ʂɖ߂�";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//10
			tmp_k.first = KEY_INPUT_ESCAPE;
			tmp_k.second = "�����I��";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//11
			tmp_k.first = KEY_INPUT_Z;
			tmp_k.second = "�}�K�W������";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//12
			tmp_k.first = KEY_INPUT_LSHIFT;
			tmp_k.second = "����";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//13
			tmp_k.first = KEY_INPUT_SPACE;
			tmp_k.second = "�W�����v";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//14
			tmp_k.first = KEY_INPUT_LCONTROL;
			tmp_k.second = "���_�ؑ�";
			tmp_k.use_mode = 0;
			this->key_use_ID.emplace_back(tmp_k);//15
			tmp_k.first = KEY_INPUT_F1;
			tmp_k.second = "�L�[�ē�";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//16
			tmp_k.first = KEY_INPUT_V;
			tmp_k.second = "���߂�";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//17
			tmp_k.first = KEY_INPUT_P;
			tmp_k.second = "�|�[�Y";
			tmp_k.use_mode = 1;
			this->key_use_ID.emplace_back(tmp_k);//18
			tmp_k.first = KEY_INPUT_H;
			tmp_k.second = "���ÃL�b�g�r�o";
			tmp_k.use_mode = 2;
			this->key_use_ID.emplace_back(tmp_k);//19
			//
			tmp_k.first = MOUSE_INPUT_LEFT;
			tmp_k.second = "�ˌ�";
			tmp_k.use_mode = 3;
			this->mouse_use_ID.emplace_back(tmp_k);//0
			tmp_k.first = MOUSE_INPUT_MIDDLE;
			tmp_k.second = "�Z���N�^�[�ؑ�";
			tmp_k.use_mode = 5;
			this->mouse_use_ID.emplace_back(tmp_k);//1
			tmp_k.first = MOUSE_INPUT_RIGHT;
			tmp_k.second = "�G�C��";
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
		//�C���t�H
		if (F1_f > 0.1f) {
			int xp_t = 100, yp_t = 300;
			int xp_sk = xp_t, yp_sk = yp_t, y_size_k = 48;
			int xp_s = 1500, yp_s = 200, y_size = 32;
			//�w�i
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*F1_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			if (F1_f > 0.9f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				keyboad.DrawExtendGraph(0, 0, 1920, 1080, true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			//�O��
			if (F1_f > 0.9f) {
				//�L�[�{�[�h�{�}�E�X�S��
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
				//�ڍ�
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
		//�펞�\��
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
		//�C���t�H
		if (F1_f > 0.1f) {
			int xp_t = 100, yp_t = 300;
			int xp_sk = xp_t, yp_sk = yp_t, y_size_k = 48;
			int xp_s = 1500, yp_s = 200, y_size = 32;
			//�w�i
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*F1_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			//�O��
			if (F1_f > 0.9f) {
				//�L�[�{�[�h�{�}�E�X�S��
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
				//�ڍ�
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
//�L�[�o�C���h
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
		//�����A�҂̓|�[�Y���j���[��
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
		//�C���t�H
		if (P_f > 0.1f) {
			int yp_t = 100;
			//�w�i
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f*P_f));
			DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
			if (P_f > 0.9f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				//�w�i�摜
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			//�O��
			if (P_f > 0.9f) {
				yp_t = 100;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "�I�v�V����", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "P�L�[�Ő퓬�ɖ߂�", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
				font24.DrawString_RIGHT(1920 - 100, yp_t, "O�L�[�ŋ����A��", GetColor(0, 255, 0)); yp_t += font24size + 30;
				//
			}
		}
		//
	}
	//
};
//���[��
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
	size_t cang_ = 0;						//�p�[�c�I��
	EnumGunParts type_ = PARTS_NONE;			//�p�[�c�̎��
	EnumAttachPoint pt_cat_ = POINTS_NONE;	//�x�[�X�p�[�c�̏ꏊ
	EnumGunParts pt_type_ = PARTS_NONE;		//�x�[�X�p�[�c�̎��
	size_t pt_sel_ = 0;						//�x�[�X�p�[�c�̔ԍ�(�}�E���g�Ȃ�)
};
//
class shaders2D {
public:
	int pshandle{ -1 }, vshandle{ -1 };
	int pscbhandle{ -1 };
	int pscbhandle2{ -1 };

	void Init(std::string vs, std::string ps) {
		this->vshandle = LoadVertexShader(("shader/" + vs).c_str());	// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		this->pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		this->pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
		this->pshandle = LoadPixelShader(("shader/" + ps).c_str());		// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
	}
	void set_dispsize(int dispx, int dispy) {
		FLOAT2 *dispsize = (FLOAT2 *)GetBufferShaderConstantBuffer(this->pscbhandle);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
		dispsize->u = float(dispx);
		dispsize->v = float(dispy);
		UpdateShaderConstantBuffer(this->pscbhandle);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
		SetShaderConstantBuffer(this->pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
	}
	void set_param(float param1, float param2, float param3, float param4) {
		FLOAT4 *f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->pscbhandle2);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->pscbhandle2);							// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
		SetShaderConstantBuffer(this->pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
	}
	void draw(VERTEX3DSHADER Screen_vertex[]) {
		SetUseVertexShader(this->vshandle);		// �g�p���钸�_�V�F�[�_�[���Z�b�g
		SetUsePixelShader(this->pshandle);		// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
		MV1SetUseOrigShader(TRUE);
		DrawPolygon3DToShader(Screen_vertex, 2);// �`��
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);					// �g�p���钸�_�V�F�[�_�[���Z�b�g
		SetUsePixelShader(-1);					// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
	}
	void draw_lamda(std::function<void()> doing) {
		SetUseVertexShader(this->vshandle);		// �g�p���钸�_�V�F�[�_�[���Z�b�g
		SetUsePixelShader(this->pshandle);		// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
		MV1SetUseOrigShader(TRUE);
		doing();
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);					// �g�p���钸�_�V�F�[�_�[���Z�b�g
		SetUsePixelShader(-1);					// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
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
	//�e�A�}�K�W�����ʃ��f��
	class Models {
	private:
		std::string name;
		std::string path;
		MV1 model;
		MV1 col;
		std::array<frames, 2> magazine_ammo_f;	//�}�K�W���p�e�t���[��
		std::array<frames, 3> LEFT_mag_frame;	//������W(�}�K�W���ێ���)
		std::array<frames, 2> magazine_f;		//�}�K�W���t���[��
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
	class damage_rad {
	public:
		float alpfa{ 1.f };
		float rad{ 0.f };
	};

	//�e�p�I�[�f�B�I
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
			shot_path = "data/audio/shot_" + getparams::_str(mdata) + ".wav";
			shot = SoundHandle::Load(shot_path);
			slide_path = "data/audio/slide_" + getparams::_str(mdata) + ".wav";
			slide = SoundHandle::Load(slide_path);
			trigger_path = "data/audio/trigger_" + getparams::_str(mdata) + ".wav";
			trigger = SoundHandle::Load(trigger_path);
			magazine_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			magazine_Set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_2.wav");
			load_ = SoundHandle::Load("data/audio/load.wav");
			sort_magazine = SoundHandle::Load("data/audio/sort.wav");
			foot_ = SoundHandle::Load("data/audio/foot_sand.wav");
			voice_damage = SoundHandle::Load("data/audio/voice/damage.wav");
			voice_death = SoundHandle::Load("data/audio/voice/death.wav");
			voice_breath = SoundHandle::Load("data/audio/voice/breath.wav");
			voice_breath_run = SoundHandle::Load("data/audio/voice/breath_run.wav");
			explosion = SoundHandle::Load("data/audio/explosion.wav");
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
	//�e�f�[�^
	class Ammos {
	private:
		std::string name;
		std::string path;
		MV1 model;
		//
		MV1 model_full;//������
		//
		float caliber = 0.f;
		float speed = 100.f;//�e��
		float pene = 10.f;//�ђ�
		int damage = 10;//�_���[�W
	public:
		auto& get_model(void) noexcept { return model; }
		const auto& get_model_full(void) const noexcept { return model_full; }
		const auto& get_caliber(void) const noexcept { return caliber; }
		const auto& get_speed(void) const noexcept { return speed; }
		const auto& get_pene(void) const noexcept { return pene; }
		const auto& get_damage(void) const noexcept { return damage; }
		auto& get_name(void) noexcept { return name; }

		auto& Set_speed(void) noexcept { return speed; }

		void Set_before(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/ammo.mv1", &this->model, true);
			MV1::Load(this->path + "/ammo2.mv1", &this->model_full, true);
		}
		void Set(void) noexcept {
			int mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//���a
			this->speed = getparams::_float(mdata);	//�e��
			this->pene = getparams::_float(mdata);	//�ђ�
			this->damage = getparams::_int(mdata);//�_���[�W
			FileRead_close(mdata);
		}

		Ammos() {
			this->caliber = 0.f;
			this->speed = 100.f;//�e��
			this->pene = 10.f;//�ђ�
			this->damage = 10;//�_���[�W
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
	//������
	class HIT_PASSIVE {
		//�_
		int hitss = 0;					/*hits�̐�*/
		std::vector<VERTEX3D> hitsver;	/*hits*/
		std::vector<DWORD> hitsind;	    /*hits*/
		int VerBuf = -1, IndexBuf = -1;	/*hits*/
		MV1 hits;						/*hits���f��*/
		GraphHandle hits_pic;			/*�摜�n���h��*/
		int IndexNum = -1, VerNum = -1;	/*hits*/
		int vnum = -1, pnum = -1;		/*hits*/
		MV1_REF_POLYGONLIST RefMesh;	/*hits*/

		bool isUPDate{ true };
	public:
		//������
		void Init(void) noexcept {
			SetUseASyncLoadFlag(FALSE);
			hits_pic = GraphHandle::Load("data/model/hit/hit.png");		 /*grass*/
			MV1::Load("data/model/hit/model.mv1", &hits, false);	//�e��
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*�Q�Ɨp���b�V���̎擾*/
		}
		//����̃��Z�b�g
		void Clear(void) noexcept {
			hitss = 0;
			vnum = 0;
			pnum = 0;
			hitsver.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsver.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
		}

		void Set(const float &caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			hitss++;
			IndexNum = RefMesh.PolygonNum * 3 * hitss;				/*�C���f�b�N�X�̐����擾*/
			VerNum = RefMesh.VertexNum * hitss;						/*���_�̐����擾*/
			hitsver.resize(VerNum);									/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.resize(IndexNum);								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			{
				float asize = 200.f*caliber;
				auto y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale)*  MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				hits.SetMatrix(mat*MATRIX_ref::Mtrans(pos));
			}
			MV1RefreshReferenceMesh(hits.get(), -1, TRUE);			/*�Q�Ɨp���b�V���̍X�V*/
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*�Q�Ɨp���b�V���̎擾*/
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
		//�_
		int hitss = 0;					/*hits�̐�*/
		std::vector<VERTEX3D> hitsver;	/*hits*/
		std::vector<DWORD> hitsind;	    /*hits*/
		int VerBuf = -1, IndexBuf = -1;	/*hits*/
		MV1 hits;						/*hits���f��*/
		GraphHandle hits_pic;			/*�摜�n���h��*/
		int IndexNum = -1, VerNum = -1;	/*hits*/
		int vnum = -1, pnum = -1;		/*hits*/
		MV1_REF_POLYGONLIST RefMesh;	/*hits*/

		bool isUPDate{ true };
	public:
		//������
		void Init(void) noexcept {
			SetUseASyncLoadFlag(FALSE);
			hits_pic = GraphHandle::Load("data/model/hit_blood/hit.png");		 /*grass*/
			MV1::Load("data/model/hit_blood/model.mv1", &hits, false);	//�e��
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*�Q�Ɨp���b�V���̎擾*/
		}
		//����̃��Z�b�g
		void Clear(void) noexcept {
			hitss = 0;
			vnum = 0;
			pnum = 0;
			hitsver.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsver.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
		}

		void Set(const float &caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			hitss++;
			IndexNum = RefMesh.PolygonNum * 3 * hitss;				/*�C���f�b�N�X�̐����擾*/
			VerNum = RefMesh.VertexNum * hitss;						/*���_�̐����擾*/
			hitsver.resize(VerNum);									/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			hitsind.resize(IndexNum);								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			{
				float asize = 200.f*caliber;
				auto y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize, asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale)*  MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				hits.SetMatrix(mat*MATRIX_ref::Mtrans(pos));
			}
			MV1RefreshReferenceMesh(hits.get(), -1, TRUE);			/*�Q�Ɨp���b�V���̍X�V*/
			RefMesh = MV1GetReferenceMesh(hits.get(), -1, TRUE);	/*�Q�Ɨp���b�V���̎擾*/
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
	class vehicles;
	//���ۂɔ��˂����e
	class BULLETS {
	private:
		//�����p��
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
		bool subHP(int sel, int damage, const std::shared_ptr<Chara>&tgt, const std::shared_ptr<Chara>& mine, std::vector<std::shared_ptr<Chara>>& chara) {
			auto q = tgt->map_col_line(this->move.repos, this->move.pos, sel);
			if (q.HitFlag) {
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

		template<class Chara>
		bool subHP(int sel, int damage, const std::shared_ptr<vehicles>&tgt, const std::shared_ptr<Chara>& mine
			//, const std::shared_ptr<vehicles>& mine_v, std::vector<std::shared_ptr<vehicles>>& vehicle
		) {
			auto q = tgt->map_col_line(this->move.repos, this->move.pos, sel);
			if (q.HitFlag) {
				this->move.pos = q.HitPosition;
				//hit
				//mine->effcs[ef_reco].Set(this->pos, q.Normal, 0.1f / 0.1f);
				mine->Set_eff(ef_hitblood, this->move.pos, q.Normal, 0.1f / 0.1f);
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
				return true;
			}
			return false;
		}

		template<class Chara, class Map>
		void UpDate(const std::shared_ptr<Chara>& mine, std::vector<std::shared_ptr<Chara>>& chara,
			const std::shared_ptr<vehicles>& mine_v, std::vector<std::shared_ptr<vehicles>>& vehicle,
			HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p,std::shared_ptr<Map>& MAPPTs) noexcept {
			if (this->Flag) {
				this->move.SetPos(this->move.pos + (this->move.mat.zvec() * (this->spec->get_speed() / FPS)));
				//����
				{
					auto p = MAPPTs->map_col_line(this->move.repos, this->move.pos);
					if (p.HitFlag) {
						this->move.pos = p.HitPosition;
					}
					//*
					for (auto& tgt : vehicle) {
						if (tgt == mine_v) {
							continue;
						}
						if (tgt->set_ref_col(this->move.repos, this->move.pos)) {
							//HEAD
							for (int j = 0; j < tgt->col.mesh_num(); j++) {
								if (subHP(j, this->spec->get_damage(), tgt, mine
									//, mine_v,vehicle
								)) {
									break;
								}
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
					if (p.HitFlag) {
						if (this->Flag) {
							this->Flag = false;
							//�e��
							mine->Set_gndhit(this->move.pos, p.Normal, 0.025f / 0.1f);
							hit_obj_p.Set(this->spec->get_caliber(), this->move.pos, p.Normal, this->move.mat.zvec());
							return;
						}
					}
					//����
					if (!this->Flag) {
						if (!p.HitFlag) {
							auto vec = (this->move.pos - this->move.repos).Norm();
							p = MAPPTs->map_col_line(this->move.pos, this->move.pos + vec * 5.f);
						}
						if (p.HitFlag) {
							mine->Set_gndhit(p.HitPosition, p.Normal, 0.025f / 0.1f);
							hit_b_obj_p.Set(0.35f, p.HitPosition, p.Normal, this->move.mat.zvec());
						}
					}
					//*/
				}
				//����(3�b�������A�X�s�[�h��0�ȉ��A�ђʂ�0�ȉ�)
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
	//�p�t�H�[�}���X
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
	//�p�[�c�f�[�^
	class GUNPARTs :public BASE_Obj {
		int type = 0;
	public:
		float recoil_xup = 0.f;				//����
		float recoil_xdn = 0.f;				//����
		float recoil_yup = 0.f;				//����
		float recoil_ydn = 0.f;				//����
		float reload_time = 1.f;			//�đ��U����
		std::vector <uint8_t> select;		//�Z���N�^�[
		std::vector <EnumSELECT_LAM> select_lam;	//���C�g�E���[�U�[
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
			//�e�L�X�g
			this->mod.Set_([&](void) noexcept {
				//���ʃf�[�^
				{
					per.name = getparams::get_str(this->mod.mdata);		//���O
					per.info = getparams::get_str(this->mod.mdata);		//����
				}
				//
				if (this->type == PARTS_MAZZULE) {
					mazzule_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == PARTS_STOCK) {
					stock_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == PARTS_BASE) {
					//�Z���N�^�[�ݒ�
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "semi") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_SEMI));	//�Z�~�I�[�g=1
						}
						else if (getparams::getright(p.c_str()) == "full") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_FULL));	//�t���I�[�g=2
						}
						else if (getparams::getright(p.c_str()) == "3b") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_3B));	//3�A�o�[�X�g=3
						}
						else if (getparams::getright(p.c_str()) == "2b") {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_2B));	//2�A�o�[�X�g=4
						}
						else {
							this->select.emplace_back(uint8_t(EnumSELECTER::SELECT_SEMI));
						}
					}
					this->audio.Set(this->mod.mdata);						//�T�E���h
					this->recoil_xup = getparams::_float(this->mod.mdata);	//����
					this->recoil_xdn = getparams::_float(this->mod.mdata);	//����
					this->recoil_yup = getparams::_float(this->mod.mdata);	//����
					this->recoil_ydn = getparams::_float(this->mod.mdata);	//����
					this->reload_time = getparams::_float(this->mod.mdata);	//�����[�h�^�C��
					Set_Ammos_data(this->mod.mdata);						//�e�f�[�^
				}
				if (this->type == PARTS_MAGAZINE) {
					this->mag_cnt = getparams::_long(this->mod.mdata);			//�e��
					Set_Ammos_data(this->mod.mdata);						//�e�f�[�^
				}
				if (this->type == PARTS_LAM) {
					//���[�U�[�����C�g��
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "laser") {
							this->select_lam.emplace_back(SELECTLAM_LASER);	//���[�U�[
						}
						else if (getparams::getright(p.c_str()) == "light") {
							this->select_lam.emplace_back(SELECTLAM_LIGHT);	//���C�g
						}
						else {
							this->select_lam.emplace_back(SELECTLAM_LASER);	//���[�U�[
						}
					}
				}
				if (this->type == PARTS_SIGHT) {
					this->zoom = getparams::_float(this->mod.mdata);
					this->reticle_size = getparams::_float(this->mod.mdata);
					this->zoom_size = getparams::_float(this->mod.mdata);
				}
				//���ʃf�[�^
				{
					per.recoil = getparams::_float(this->mod.mdata);		//���R�C��
					per.weight = getparams::_float(this->mod.mdata);		//�d��
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
				//�t���[��
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
	//��i�f�[�^
	class Meds :public BASE_Obj {
	public:
		int repair = 0;
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//�e�L�X�g
			this->mod.Set_([&](void) noexcept {
				this->repair = getparams::_long(this->mod.mdata);//
			});
		}
	};

	//��i�f�[�^
	class Grenades :public BASE_Obj {
	public:
		float time = 0;
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//�e�L�X�g
			this->mod.Set_([&](void) noexcept {
				this->time = getparams::_float(this->mod.mdata);//
			});
		}
	};
	//�A�C�e��
	class Items {
	private:
		//����
		moves move;
		MV1 obj;
		float del_timer = 0.f;
		size_t id_t = 0;
		//�}�K�W����p�p�����[�^�[
		GUNPARTs* ptr_mag{ nullptr };
		GUNPARTs magazine_param;
		//���ÃL�b�g��p�p�����[�^�[
		Meds* ptr_med{ nullptr };
		Meds medkit;
		//���ÃL�b�g��p�p�����[�^�[
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
					if (pp.HitFlag) {
						this->move.HitGround(pp, 0.005f);
						auto fvec = this->move.vec.Norm();
						auto nvec = VECTOR_ref(pp.Normal).Norm();
						this->move.vec = (fvec + nvec * ((fvec*-1.f).dot(nvec)*2.f))*(this->move.vec.size()*0.5f);
					}
				}
				else {
					auto pp = MAPPTs->map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (pp.HitFlag) {
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
					if (p.HitFlag) {
						EndPos = p.HitPosition;
					}
					zz = (Segment_Point_MinLength(StartPos.get(), EndPos.get(), this->move.pos.get()) <= 0.4f);
				}
				//��
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
				if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.3f, 0)).HitFlag &&
					MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.0f, 0)).HitFlag) {
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
				//�O�����j
				this->Detach_item();
				for (auto& tgt : chara) {
					tgt->calc_gredamage(mine, chara);
					if (tgt->get_alive()) {
						float scale = (this->move.pos - tgt->get_head_pos()).size();
						if (scale < 10.f) {
							if (!MAPPTs->map_col_line(this->move.pos, tgt->get_head_pos()).HitFlag) {
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

		// �R�s�[�͋֎~���邪�A���[�u�͋�����
		Items(const Items&) = delete;
		Items& operator=(const Items&) = delete;

		// ���ꃁ���o�֐��𖾎��I�ɒ�`��������delete�����ꍇ�A
		// ����ȊO�̓��ꃁ���o�֐��͖����I�ɒ�`��������default�錾���Ȃ����
		// �Öْ�`����Ȃ�
		Items(Items&&) = default;
		Items() = default;
		Items& operator=(Items&&) = default;

		~Items() {}
	};
	//
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
				this->frame1.second -= obj.frame(int(p2)); //�e�����鎞�����Ƃ�
			}
			if (obj.frame_child_num(this->frame1.first) > 0) {
				if (obj.frame_name(this->frame1.first + 1).find("�p", 0) != std::string::npos) {
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
	class foot_frame {
	public:
		//�ύX�s��
		frames frame;
		//�v�Z�p(���R)
		float will_y=0.f;
	};
	class cat_frame {
	public:
		//�ύX�s��
		frames frame;
		//�v�Z�p(���R)
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
	class Vehcs {
	public:
		//����
		std::string name;				  /**/
		MV1 obj, col;					  /**/
		VECTOR_ref minpos, maxpos;			  /**/
		std::vector<gun_frame> gunframe;			  /**/
		std::vector<foot_frame> wheelframe;			  /**/
		std::vector<foot_frame> wheelframe_nospring;		  /*�U���։�]*/
		uint16_t HP = 0;					  /**/
		std::vector<std::pair<size_t, float>> armer_mesh; /*���bID*/
		std::vector<size_t> space_mesh;			  /*���bID*/
		std::vector<size_t> module_mesh;		  /*���bID*/
		int camo_tex = 0;				  /**/
		std::vector<int> camog;				  /**/
		bool isfloat = false;			  /*�������ǂ���*/
		float down_in_water = 0.f;			  /*���ޔ���ӏ�*/
		float max_speed_limit = 0.f;			  /*�ō����x(km/h)*/
		float mid_speed_limit = 0.f;			  /*���s���x(km/h)*/
		float min_speed_limit = 0.f;			  /*�������x(km/h)*/
		float flont_speed_limit = 0.f;			  /*�O�i���x(km/h)*/
		float back_speed_limit = 0.f;			  /*��ޑ��x(km/h)*/
		float body_rad_limit = 0.f;			  /*���񑬓x(�x/�b)*/
		float turret_rad_limit = 0.f;			  /*�C���쓮���x(�x/�b)*/
		frames fps_view;//�R�b�N�s�b�g
		GraphHandle ui_pic;//�V���G�b�g
		int pic_x, pic_y;//�T�C�Y
		//���
		std::array<int, 4> square{ 0 };//���p�̎l��
		std::array<std::vector<frames>, 2> b2upsideframe; /*���я�*/
		std::array<std::vector<cat_frame>, 2> b2downsideframe; /*����*/
		//
		void into(const Vehcs& t) {
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
			this->camo_tex = t.camo_tex;
			this->camog = t.camog;
			this->isfloat = t.isfloat;
			this->down_in_water = t.down_in_water;
			this->max_speed_limit = t.max_speed_limit;
			this->mid_speed_limit = t.mid_speed_limit;
			this->min_speed_limit = t.min_speed_limit;
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
		//���O�ǂݍ���
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
		//���C���ǂݍ���
		static void set_vehicles(std::vector<Mainclass::Vehcs>* veh_) {
			using namespace std::literals;
			//����
			for (auto& t : (*veh_)) {
				//���e�X�g
				t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				GetGraphSize(t.ui_pic.get(), &t.pic_x, &t.pic_y);
				//�ŗL
				t.down_in_water = 0.f;
				for (int i = 0; i < t.obj.mesh_num(); i++) {
					auto p = t.obj.mesh_maxpos(i).y();
					if (t.down_in_water < p) {
						t.down_in_water = p;
					}
				}
				t.down_in_water /= 2.f;
				for (int i = 0; i < t.obj.frame_num(); i++) {
					std::string p = t.obj.frame_name(i);
					if (p.find("�]��", 0) != std::string::npos) {
						t.wheelframe.resize(t.wheelframe.size() + 1);
						t.wheelframe.back().frame = { i,t.obj.frame(i) };
					}
					else if ((p.find("��", 0) != std::string::npos) && (p.find("�]��", 0) == std::string::npos)) {
						t.wheelframe_nospring.resize(t.wheelframe_nospring.size() + 1);
						t.wheelframe_nospring.back().frame = { i,t.obj.frame(i) };
					}
					else if (p.find("����", 0) != std::string::npos) {
						t.gunframe.resize(t.gunframe.size() + 1);
						t.gunframe.back().set(t.obj, i);
					}
					else if (p=="min") {
						t.minpos = t.obj.frame(i);
					}
					else if (p == "max") {
						t.maxpos = t.obj.frame(i);
					}
					else if (p.find("�QD����", 0) != std::string::npos || p.find("2D����", 0) != std::string::npos) { //2D����
						t.b2upsideframe[0].clear();
						t.b2upsideframe[1].clear();
						for (int z = 0; z < t.obj.frame_child_num(i); z++) {
							if (t.obj.frame(i + 1 + z).x() > 0) {
								t.b2upsideframe[0].resize(t.b2upsideframe[0].size() + 1);
								t.b2upsideframe[0].back() = { i + 1 + z, t.obj.frame(i + 1 + z) };
							}
							else {
								t.b2upsideframe[1].resize(t.b2upsideframe[1].size() + 1);
								t.b2upsideframe[1].back() = { i + 1 + z, t.obj.frame(i + 1 + z) };
							}
						}
						std::sort(t.b2upsideframe[0].begin(), t.b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //�\�[�g
						std::sort(t.b2upsideframe[1].begin(), t.b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //�\�[�g
					}
					else if (p.find("���ѐݒu��", 0) != std::string::npos) { //2D����
						t.b2downsideframe[0].clear();
						t.b2downsideframe[1].clear();
						for (int z = 0; z < t.obj.frame_child_num(i); z++) {
							if (t.obj.frame(i + 1 + z).x() > 0) {
								t.b2downsideframe[0].resize(t.b2downsideframe[0].size() + 1);
								t.b2downsideframe[0].back().frame = { i + 1 + z, t.obj.frame(i + 1 + z) };
							}
							else {
								t.b2downsideframe[1].resize(t.b2downsideframe[1].size() + 1);
								t.b2downsideframe[1].back().frame = { i + 1 + z, t.obj.frame(i + 1 + z) };
							}
						}
					}

				}
				//2	���㕔0
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (f.frame.second.x() >= 0) {
							t.square[0] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[0] != f.frame.first) {
							if (f.frame.second.x() >= 0) {
								if (tmp < f.frame.second.z()) {
									t.square[0] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//10	���O��1
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (f.frame.second.x() >= 0) {
							t.square[1] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[1] != f.frame.first) {
							if (f.frame.second.x() >= 0) {
								if (tmp > f.frame.second.z()) {
									t.square[1] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//3	�E�㕔2
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (!(f.frame.second.x() >= 0)) {
							t.square[2] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[2] != f.frame.first) {
							if (!(f.frame.second.x() >= 0)) {
								if (tmp < f.frame.second.z()) {
									t.square[2] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//11	�E�O��3
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (!(f.frame.second.x() >= 0)) {
							t.square[3] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[3] != f.frame.first) {
							if (!(f.frame.second.x() >= 0)) {
								if (tmp > f.frame.second.z()) {
									t.square[3] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//���b
				for (int i = 0; i < t.col.mesh_num(); i++) {
					std::string p = t.col.material_name(i);
					if (p.find("armer", 0) != std::string::npos) {
						t.armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//���b
					}
					else if (p.find("space", 0) != std::string::npos) {
						t.space_mesh.emplace_back(i);//��ԑ��b
					}
					else {
						t.module_mesh.emplace_back(i);//���W���[��
					}
				}
				//����
				{
					t.camo_tex = -1;
					for (int i = 0; i < MV1GetTextureNum(t.obj.get()); i++) {
						std::string p = MV1GetTextureName(t.obj.get(), i);
						if (p.find("b.", 0) != std::string::npos || p.find("B.", 0) != std::string::npos) {
							t.camo_tex = i;
							break;
						}
					}
					SetUseTransColor(FALSE);
					WIN32_FIND_DATA win32fdt;
					HANDLE hFind;
					hFind = FindFirstFile(("data/tank/"s + t.name + "/B*.jpg").c_str(), &win32fdt);
					if (hFind != INVALID_HANDLE_VALUE) {
						do {
							if (win32fdt.cFileName[0] != '.') {
								t.camog.resize(t.camog.size() + 1);
								t.camog.back() = MV1LoadTexture(("data/tank/"s + t.name + "/" + win32fdt.cFileName).c_str());
							}
						} while (FindNextFile(hFind, &win32fdt));
					} //else{ return false; }
					FindClose(hFind);
					SetUseTransColor(TRUE);
				}
				//data
				{
					int mdata = FileRead_open(("data/tank/" + t.name + "/data.txt").c_str(), FALSE);
					t.isfloat = getparams::_bool(mdata);
					t.flont_speed_limit = getparams::_float(mdata);
					t.back_speed_limit = getparams::_float(mdata);
					t.body_rad_limit = getparams::_float(mdata);
					t.turret_rad_limit = getparams::_float(mdata);
					t.HP = uint16_t(getparams::_ulong(mdata));
					auto stt = getparams::get_str(mdata);
					for (auto& g : t.gunframe) {
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
					FileRead_close(mdata);
					for (auto& g : t.gunframe) {
						g.Set_Ammos_after();
					}
				}
			}
		}
	};

	class ammos {
	public:
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Mainclass::Ammos spec;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;
	public:
		void Put(const Mainclass::Ammos& spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->spec = spec_t;
			this->spec.Set_speed() *= float(75 + GetRand(50)) / 100.f;//�����_��
			this->pos = pos_t;
			this->vec = vec_t;
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->yadd = 0.f;
			this->repos = this->pos;
		}
	};
	class Guns {							/**/
		//size_t usebullet{};					/*�g�p�e*/
		//std::array<ammos, 64> bullet;		/*�m�ۂ���e*/
		size_t use_bullet{ 0 };							/*�m�ۂ���e*/
		std::array<BULLETS, max_bullet> bullet;			/*�m�ۂ���e*/

		float loadcnt{ 0 };					/*���Ă�J�E���^�[*/
		float fired{ 0.f };					/*���ސ�*/
		int16_t rounds{ 0 };				/*�e��*/
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
			//for (auto& a : this->bullet) {
			//}
			//this->gun_info;
			this->Spec.clear();
		}
		void Set(const gun_frame& resorce) {
			this->gun_info = resorce;
			this->rounds = this->gun_info.rounds;
			//�g�p�C�e
			this->Spec = this->gun_info.Spec;
			for (auto& a : this->bullet) { a.Set(); }			//�e
		}
		void SetGunRad(const float view_xrad, const float view_yrad,float limit) {
			this->gun_info.yrad += std::clamp(view_yrad, -limit, limit);
			//this->gun_info.yrad = std::clamp(this->gun_info.yrad + std::clamp(view_yrad, -limit, limit),deg2rad(-30.0)+yrad,deg2rad(30.0)+yrad);//�ˊE����
			this->gun_info.xrad = std::clamp(this->gun_info.xrad + std::clamp(view_xrad, -limit, limit), deg2rad(-10), deg2rad(20));
		}
		template<class Chara, class Map>
		void UpDate(bool key, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t,
			const std::shared_ptr<Chara>& mine, std::vector<std::shared_ptr<Chara>>& chara,
			const std::shared_ptr<vehicles>& mine_v, std::vector<std::shared_ptr<vehicles>>& vehicle,
			HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p, std::shared_ptr<Map>& MAPPTs) {
			if (key && this->loadcnt == 0 && this->rounds > 0) {
				auto& u = this->bullet[this->use_bullet];
				++this->use_bullet %= this->bullet.size();
				//�R�R�����ω�
				moves tmp;
				tmp.pos = pos_t;
				tmp.mat = MATRIX_ref::RotVec2(VGet(0,0,1.f), vec_t);

				u.Put(&this->Spec[0], tmp);
				this->loadcnt = this->Getgun_info().load_time;
				this->rounds = std::max<uint16_t>(this->rounds - 1, 0);
				this->fired = 1.f;
				mine->Set_eff(ef_fire, pos_t, vec_t, 0.1f / 0.1f);//�m�[�}��
				/*
				if (u.spec.type_a != 2) {
					set_effect(&c.effcs[ef_fire], this->obj.frame(this->Getgun_info().frame3.first), u.vec, u.spec.caliber_a / 0.1f);
					if (u.spec.caliber_a >= 0.037f) {
						set_effect(&c.effcs_gun[c.gun_effcnt].first, this->obj.frame(this->Getgun_info().frame3.first), u.vec);
						set_pos_effect(&c.effcs_gun[c.gun_effcnt].first, Drawparts->get_effHandle(ef_smoke2));
						c.effcs_gun[c.gun_effcnt].second = &u;
						c.effcs_gun[c.gun_effcnt].cnt = 0;
						++c.gun_effcnt %= c.effcs_gun.size();
					}
				}
				else {
					set_effect(&c.effcs_missile[c.missile_effcnt].first, this->obj.frame(this->Getgun_info().frame3.first), u.vec);
					set_pos_effect(&c.effcs_missile[c.missile_effcnt].first, Drawparts->get_effHandle(ef_smoke1));
					c.effcs_missile[c.missile_effcnt].second = &u;
					c.effcs_missile[c.missile_effcnt].cnt = 0;
					++c.missile_effcnt %= c.effcs_missile.size();
				}
				*/
			}
			this->loadcnt = std::max(this->loadcnt - 1.f / FPS, 0.f);
			this->fired = std::max(this->fired - 1.f / FPS, 0.f);

			//�e�̏���
			for (auto& a : this->bullet) { a.UpDate(mine, chara, mine_v, vehicle, hit_obj_p, hit_b_obj_p, MAPPTs); }
		}
		/*�e���`��*/
		void Draw_ammo(void) noexcept {
			for (auto& a : this->bullet) { a.Draw(); }
		}
		/* UI�����Ƀq�b�g������\��*/
		void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
			for (auto& a : this->bullet) {
				if (a.hit_alpha >= 10.f / 255.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f*a.hit_alpha));
					hit_Graph.DrawRotaGraph(a.hit_window_x, a.hit_window_y, a.hit_alpha*0.5f, 0.f, true);//(ismine ? 1.f : 0.5f)
				}
			}
		}

		Guns() {
		}
		Guns(const Guns &) {
		}
	};										/**/
	class b2Pats {
		b2FixtureDef fixtureDef;			/*���I�{�f�B�t�B�N�X�`�����`���܂�*/
		std::unique_ptr<b2Body> body;		/**/
		b2Fixture* playerfix{ nullptr };	/**/
	public:
		VECTOR_ref pos;/*��*/

		void Set(b2Body* body_ptr, b2Shape* dynamicBox) {
			fixtureDef.shape = dynamicBox;								/**/
			fixtureDef.density = 1.0f;									/*�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�*/
			fixtureDef.friction = 0.3f;									/*�f�t�H���g�̖��C���I�[�o�[���C�h*/
			this->body.reset(body_ptr);									/**/
			this->playerfix = this->body->CreateFixture(&fixtureDef);	/*�V�F�C�v���{�f�B�ɒǉ�*/
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
	struct FootWorld {
		b2World* world = nullptr;			     /*��world*/
		b2RevoluteJointDef f_jointDef;	     /*�W���C���g*/
		std::vector<b2Pats> Foot, Wheel, Yudo; /**/
	};

	struct wallPats {
		b2Pats b2;
		std::array<VECTOR_ref, 2> pos;
	};

	typedef std::pair<size_t, float> pair_hit;

	class vehicles : public std::enable_shared_from_this<vehicles> {
	private:
		//size_t use_id = 0;															//�g�p����ԗ�(�@��)
		Vehcs use_veh;																/*�ŗL�l*/

		b2Pats b2mine;																/*box2d*/
		float spd_buf = 0.f;														/*box2d*/

		MV1 obj;																	/**/
		bool hit_check = false;														//�����蔻�����邩�`�F�b�N
	public:
		MV1 col;																	/**/
		moves move;
	private:
		std::vector<Guns> Gun_;														/**/
		float speed = 0.f, speed_add = 0.f, speed_sub = 0.f;						/**/
		float xrad = 0.f, xradadd = 0.f, xradadd_left = 0.f, xradadd_right = 0.f;	/**/
		float yrad = 0.f, yradadd = 0.f, yradadd_left = 0.f, yradadd_right = 0.f;	/**/
		float zrad = 0.f, zradadd = 0.f, zradadd_left = 0.f, zradadd_right = 0.f;	/**/
		std::vector<MV1_COLL_RESULT_POLY> hitres;									/*�m��*/
		float wheel_Left = 0.f, wheel_Right = 0.f;									//�]�։�]
		float wheel_Leftadd = 0.f, wheel_Rightadd = 0.f;							//�]�։�]
		std::vector<pair_hit> hitssort;												/*�t���[���ɓ�����������*/

		std::array<FootWorld, 2> foot; /*��*/
		float per_sus{ 0.f };
		float view_xrad = 0.f;
		float view_yrad = 0.f;
		float xrad_shot = 0.f; //�ˌ�����x
		float zrad_shot = 0.f; //�ˌ�����z
		VECTOR_ref wheel_normal = VGet(0.f, 1.f, 0.f);
		std::array<bool, 15> key{ false };	//�L�[
		bool nearhit{ false };							//

		float body_yrad{ 0.f };							//���̊p�x

	public:
		unsigned int got_damage_color{ 0 };
		int got_damage_x{ 0 };
		float got_damage_f{ 0.f };
		std::vector<damage_rad> got_damage_;
		int got_damage{ 0 };
		int HP{ 100 };
		float HP_r{ 100.f };
		int HP_full{ 100 };
		std::vector<int> HP_parts;
		float spd_rec;

		const auto& get_body_yrad(void) const noexcept { return this->body_yrad; }														//�~�j�}�b�v�p

		const MV1_COLL_RESULT_POLY map_col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel) const noexcept {
			return col.CollCheck_Line(StartPos, EndPos, -1, sel);
		}
		/*�J�����w��*/
		template<class Y>
		void Set_cam(std::shared_ptr<Y>& MAPPTs, cam_info& camera_main,const VECTOR_ref& eyevec, const float fov_) noexcept {
			float range = 7.5f;
			VECTOR_ref eyepos = this->move.pos + (this->move.mat.yvec()*3.f) + eyevec * range;
			VECTOR_ref eyetgt = eyepos + eyevec * (-range);

			if (MAPPTs->map_col_nearest(eyepos, &eyetgt)) {
				eyepos = eyetgt;
			}

			camera_main.set_cam_pos(eyepos, eyepos + eyevec * -1.f, this->move.mat.yvec());
			camera_main.near_ = 0.1f;
			easing_set(&camera_main.fov, fov_, 0.9f);
		}

		void reset() {
			//*
			this->obj.Dispose();
			this->col.Dispose();
			this->hit_check = false;
			this->HP = 0;
			this->speed_add = 0.f;
			this->speed_sub = 0.f;
			this->speed = 0.f;
			this->move.vec.clear();
			this->hitres.clear();
			this->HP_parts.clear();
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
			//this->use_veh;
			for (auto& g : this->Gun_) { g.Clear(); }
			this->Gun_.clear();
			for (auto& h : this->hitssort) {
				h.first = 0;
				h.second = 0.f;
			}
			this->hitssort.clear();						//�t���[���ɓ�����������
			//*/
		}

		void Set(const Vehcs& tgt, std::unique_ptr<b2World>& world, const EffekseerEffectHandle& gndsmkHndle) {
			//�����ʒu
			this->move.pos = VGet(1.f, 10.f, 0);
			this->move.mat = MATRIX_ref::RotY(deg2rad(0.f));
			//
			this->use_veh.into(tgt);
			this->obj = tgt.obj.Duplicate();
			this->col = tgt.col.Duplicate();
			this->obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			//��ԕ���set
			b2PolygonShape dynamicBox; /*�_�C�i�~�b�N�{�f�B�ɕʂ̃{�b�N�X�V�F�C�v���`���܂��B*/
			dynamicBox.SetAsBox(
				(this->use_veh.maxpos.x() - this->use_veh.minpos.x()) / 2, (this->use_veh.maxpos.z() - this->use_veh.minpos.z()) / 2,
				b2Vec2((this->use_veh.minpos.x() + this->use_veh.maxpos.x()) / 2,
				-(this->use_veh.minpos.z() + this->use_veh.maxpos.z()) / 2), 0.f);
			b2BodyDef bodyDef;														/*�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo��*/
			bodyDef.type = b2_dynamicBody;											/**/
			bodyDef.position.Set(this->move.pos.x(), this->move.pos.z());						/**/
			bodyDef.angle = atan2f(-this->move.mat.zvec().x(), -this->move.mat.zvec().z());	/**/
			this->b2mine.Set(world->CreateBody(&bodyDef), &dynamicBox);				/*�V�F�C�v���{�f�B�ɒǉ�*/
								/* ���̂�ێ�����уV�~�����[�g���郏�[���h�I�u�W�F�N�g���\�z*/
			for (size_t k = 0; k < 2; ++k) {
				auto& f = foot[k];
				f.world = new b2World(b2Vec2(0.0f, 0.0f));
				{
					b2Body* ground = NULL;
					{
						b2BodyDef bd;
						ground = f.world->CreateBody(&bd);
						b2EdgeShape shape;
						shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
						ground->CreateFixture(&shape, 0.0f);
					}
					b2Body* prevBody = ground;

					//����
					f.Foot.clear();
					{
						VECTOR_ref vects;
						for (auto& w : this->use_veh.b2upsideframe[k]) {
							vects = w.second;
							if (vects.x() * ((k == 0) ? 1 : -1) > 0) {
								f.Foot.resize(f.Foot.size() + 1);
								b2PolygonShape f_dynamicBox; /*�_�C�i�~�b�N�{�f�B�ɕʂ̃{�b�N�X�V�F�C�v���`���܂��B*/
								f_dynamicBox.SetAsBox(0.2f, 0.05f);
								b2BodyDef f_bodyDef;
								f_bodyDef.type = b2_dynamicBody;
								f_bodyDef.position.Set(vects.z(), vects.y());
								f.Foot.back().Set(f.world->CreateBody(&f_bodyDef), &f_dynamicBox);
								f.f_jointDef.Initialize(prevBody, f.Foot.back().Get(), b2Vec2(vects.z(), vects.y()));
								f.world->CreateJoint(&f.f_jointDef);
								prevBody = f.Foot.back().Get();
							}
						}
						if (f.Foot.size() != 0) {
							f.f_jointDef.Initialize(prevBody, ground, b2Vec2(vects.z(), vects.y()));
							f.world->CreateJoint(&f.f_jointDef);
						}
					}

					//�]��(����)
					f.Wheel.clear();
					f.Yudo.clear();
					if (f.Foot.size() != 0) {
						for (auto& w : this->use_veh.wheelframe) {
							VECTOR_ref vects = VECTOR_ref(VTransform(VGet(0, 0, 0), this->obj.GetFrameLocalMatrix(w.frame.first).get()));
							if (vects.x() * ((k == 0) ? 1 : -1) > 0) {
								f.Wheel.resize(f.Wheel.size() + 1);
								b2CircleShape shape;
								shape.m_radius = VTransform(VGet(0, 0, 0), this->obj.GetFrameLocalMatrix(w.frame.first).get()).y - 0.1f;
								b2BodyDef fw_bodyDef;
								fw_bodyDef.type = b2_kinematicBody;
								fw_bodyDef.position.Set(vects.z(), vects.y());
								f.Wheel.back().Set(f.world->CreateBody(&fw_bodyDef), &shape);							}
						}
						//�U����(�����Ȃ�)
						for (auto& w : this->use_veh.wheelframe_nospring) {
							VECTOR_ref vects = VTransform(VGet(0, 0, 0), this->obj.GetFrameLocalMatrix(w.frame.first).get());
							if (vects.x() * ((k == 0) ? 1 : -1) > 0) {
								f.Yudo.resize(f.Yudo.size() + 1);
								b2CircleShape shape;
								shape.m_radius = 0.05f;
								b2BodyDef fy_bodyDef;
								fy_bodyDef.type = b2_kinematicBody;
								fy_bodyDef.position.Set(vects.z(), vects.y());
								f.Yudo.back().Set(f.world->CreateBody(&fy_bodyDef), &shape);
							}
						}
					}
				}
			}
			//�R���W����
			for (int j = 0; j < this->col.mesh_num(); j++) {
				this->col.SetupCollInfo(8, 8, 8, -1, j);
			}
			this->hitres.resize(this->col.mesh_num());		//���W���[�����Ƃ̓����蔻�茋�ʂ��m��
			this->hitssort.resize(this->col.mesh_num());	//���W���[�����Ƃ̓����蔻�菇�����m��
			//
			for (int j = 0; j < this->obj.material_num(); ++j) {
				MV1SetMaterialSpcColor(this->obj.get(), j, GetColorF(0.85f, 0.82f, 0.78f, 0.1f));
				MV1SetMaterialSpcPower(this->obj.get(), j, 50.0f);
			}
			//�C
			{
				this->Gun_.resize(this->use_veh.gunframe.size());
				for (int j = 0; j < this->Gun_.size(); j++) {
					this->Gun_[j].Set(this->use_veh.gunframe[j]);
				}
			}
			//�q�b�g�|�C���g
			this->HP = this->use_veh.HP;
			//���W���[���ϋv
			this->HP_parts.resize(this->col.mesh_num());
			for (auto& h : this->HP_parts) {
				h = 100;
			}

			for (auto& g : this->use_veh.b2downsideframe) {
				for (auto& t : g) {
					t.gndsmkeffcs.set_loop(gndsmkHndle);
					t.gndsmksize = 0.1f;
				}
			}
		}

		void BodyFrameLocalMatrix(MV1&obj_body,const frames& id, const MATRIX_ref& mat_t = MGetIdent()) const noexcept { obj_body.SetFrameLocalMatrix(id.first, mat_t*MATRIX_ref::Mtrans(id.second)); }

		template<class Chara, class Map>
		void UpDate(const cam_info& cams,
			const std::shared_ptr<Chara>& mine, std::vector<std::shared_ptr<Chara>>& chara,
			std::vector<std::shared_ptr<vehicles>>& vehicle,
			HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p, std::shared_ptr<Map>& MAPPTs) {

			if (vehicle[0] == shared_from_this()) {
				key[0] = (CheckHitKey(KEY_INPUT_SPACE) != 0);//((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);   //�ˌ�
				key[1] = ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0); //�}�V���K��
				key[2] = (CheckHitKey(KEY_INPUT_W) != 0);				//�O�i
				key[3] = (CheckHitKey(KEY_INPUT_S) != 0);				//���
				key[4] = (CheckHitKey(KEY_INPUT_D) != 0);				//�E
				key[5] = (CheckHitKey(KEY_INPUT_A) != 0);				//��
				{
					if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) { //�C�����b�N
						view_yrad = 0.f;
						view_xrad = 0.f;
					}
					else {
						//�_��
						VECTOR_ref vec_a;
						{
							VECTOR_ref endpos = cams.campos + (cams.camvec - cams.campos).Norm() * (100.f);
							//�}�b�v�ɓ���������
							//mapparts->map_col_line_nearest(campos, &endpos);

							vec_a = (this->obj.frame(this->Gun_[0].Getgun_info().frame2.first) - endpos).Norm();
						}
						//���f
						auto vec_z = this->obj.frame(this->Gun_[0].Getgun_info().frame3.first) - this->obj.frame(this->Gun_[0].Getgun_info().frame2.first);
						float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
						float a_hyp = std::hypotf(vec_a.x(), vec_a.z());
						float cost = (vec_a.z() * vec_z.x() - vec_a.x() * vec_z.z()) / (a_hyp * z_hyp);

						view_yrad = (atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / 5.f; //cos�擾2D
						view_xrad = (atan2f(-vec_z.y(), z_hyp) - atan2f(vec_a.y(), a_hyp)) / 5.f;
					}
				}
			}
			else {
				view_yrad = 0.f;
				view_xrad = 0.f;
			}
			//��ԉ��Z
			{
				auto yrad_p = 0.f;
				spd_rec = this->speed;
				this->nearhit = false;
				//�C������
				{
					//�p�x�w��
					float limit = deg2rad(this->use_veh.turret_rad_limit) / FPS;
					for (auto& g : this->Gun_) {
						g.SetGunRad(view_xrad, view_yrad, limit);
					}
					//���f
					for (auto& g : this->Gun_) {
						if (g.Getgun_info().frame1.first > 0) {
							BodyFrameLocalMatrix(this->obj, g.Getgun_info().frame1, MATRIX_ref::RotY(g.Getgun_info().yrad));
							BodyFrameLocalMatrix(this->col, g.Getgun_info().frame1, MATRIX_ref::RotY(g.Getgun_info().yrad));
						}
						if (g.Getgun_info().frame2.first > 0) {
							BodyFrameLocalMatrix(this->obj, g.Getgun_info().frame2, MATRIX_ref::RotX(g.Getgun_info().xrad));
							BodyFrameLocalMatrix(this->col, g.Getgun_info().frame2, MATRIX_ref::RotX(g.Getgun_info().xrad));
						}
						if (g.Getgun_info().frame3.first > 0) {
							BodyFrameLocalMatrix(this->obj, g.Getgun_info().frame3, MATRIX_ref::Mtrans(VGet(0.f, 0.f, g.Getfired() * 0.5f)));
							BodyFrameLocalMatrix(this->col, g.Getgun_info().frame3, MATRIX_ref::Mtrans(VGet(0.f, 0.f, g.Getfired() * 0.5f)));
						}
					}
				}
				//�]��
				{
					auto y_vec = this->move.mat.yvec();
					for (auto& f : this->use_veh.wheelframe) {
						MATRIX_ref tmp;
						this->obj.frame_reset(f.frame.first);
						auto startpos = this->obj.frame(f.frame.first);
						auto hp2 = MAPPTs->map_col_line(startpos + y_vec * ((-f.frame.second.y()) + 2.f), startpos + y_vec * ((-f.frame.second.y()) - 0.3f));

						if (hp2.HitFlag == TRUE) {
							easing_set(&f.will_y, hp2.HitPosition.y + y_vec.y() * f.frame.second.y() - startpos.y(), 0.9f);
						}
						else {
							easing_set(&f.will_y, -0.3f, 0.9f);
						}
						tmp = MATRIX_ref::Mtrans(VGet(0.f, f.will_y, 0.f));

						this->obj.SetFrameLocalMatrix(f.frame.first, MATRIX_ref::RotX((f.frame.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * tmp * MATRIX_ref::Mtrans(f.frame.second));
					}
					for (auto& f : this->use_veh.wheelframe_nospring) {
						this->obj.SetFrameLocalMatrix(f.frame.first, MATRIX_ref::RotX((f.frame.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * MATRIX_ref::Mtrans(f.frame.second));
					}
				}
				easing_set(
					&wheel_normal,
					((this->obj.frame(this->use_veh.square[0]) - this->obj.frame(this->use_veh.square[3])).cross(this->obj.frame(this->use_veh.square[1]) - this->obj.frame(this->use_veh.square[2]))).Norm(),
					0.95f);
				//����
				{
					for (auto& g : this->use_veh.b2downsideframe) {
						for (auto& t : g) {
							MATRIX_ref tmp;
							this->obj.frame_reset(t.frame.first);
							auto startpos = this->obj.frame(t.frame.first);
							t.colres = MAPPTs->map_col_line(startpos + this->move.mat.yvec() * ((-t.frame.second.y()) + 2.f), startpos + this->move.mat.yvec() * ((-t.frame.second.y()) - 0.3f));
							if (t.colres.HitFlag == TRUE) {
								tmp = MATRIX_ref::Mtrans(VGet(0.f, t.colres.HitPosition.y + this->move.mat.yvec().y() * t.frame.second.y() - startpos.y(), 0.f));
							}
							else {
								tmp = MATRIX_ref::Mtrans(VGet(0.f, -0.4f, 0.f));
							}
							this->obj.SetFrameLocalMatrix(t.frame.first, tmp * MATRIX_ref::Mtrans(t.frame.second));
						}
					}
				}
				//�ړ�
				auto isfloat = (this->move.pos.y() == -this->use_veh.down_in_water);
				//�s��
				{
					//���Z�b�g
					{
						const auto yvec = (this->use_veh.isfloat && isfloat) ? VGet(0.f, 1.f, 0.f) : wheel_normal;
						auto pp = (this->move.mat*MATRIX_ref::RotVec2(VGet(0.f, 1.f, 0.f), yvec).Inverse()).zvec()*-1.f;

						yrad_p = atan2f(pp.x(), pp.z());
						//printfDx("%5.2f\n", rad2deg(yrad_p));

						const auto zvec = MATRIX_ref::Vtrans(VGet(-sinf(yrad_p), 0.f, -cosf(yrad_p)), MATRIX_ref::RotVec2(VGet(0.f, 1.f, 0.f), yvec));
						this->move.mat = MATRIX_ref::Axis1_YZ(yvec, zvec);
					}
					//�n�ʔ���
					{
						auto yp = this->move.pos.y();
						float hight_t = 0.f;
						int cnt_t = 0;
						bool tatch = false;
						for (auto& g : this->use_veh.b2downsideframe) {
							for (auto& t : g) {
								hight_t += this->obj.frame(t.frame.first).y();
								cnt_t++;
								if (t.colres.HitFlag == TRUE) {
									tatch = true;
								}
							}
						}
						//*
						for (auto& s : this->use_veh.square) {
							auto p_t = this->obj.frame(s);
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

						//�n��or���ʂɂ��邩�ǂ���
						if (tatch || (this->use_veh.isfloat && isfloat)) {
							//�O�i���
							{
								const auto old = this->speed_add + this->speed_sub;
								if (key[2]) {
									this->speed_add = (this->speed_add < (this->use_veh.flont_speed_limit / 3.6f)) ? (this->speed_add + (0.06f / 3.6f) * (60.f / FPS)) : this->speed_add;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.7f / 3.6f) * (60.f / FPS)) : this->speed_sub;
								}
								if (key[3]) {
									this->speed_sub = (this->speed_sub > (this->use_veh.back_speed_limit / 3.6f)) ? (this->speed_sub - (0.06f / 3.6f) * (60.f / FPS)) : this->speed_sub;
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.7f / 3.6f) * (60.f / FPS)) : this->speed_add;
								}
								if (!(key[2]) && !(key[3])) {
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
								}
								this->speed = (old + ((this->speed_add + this->speed_sub) - old) * 0.1f) / FPS;
								this->move.vec = this->move.mat.zvec() * -this->speed;
							}
							//����
							{
								this->yradadd_left = (key[4]) ? std::max(this->yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->use_veh.body_rad_limit)) : std::min(this->yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd_right = (key[5]) ? std::min(this->yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->use_veh.body_rad_limit)) : std::max(this->yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd = (this->yradadd_left + this->yradadd_right) / FPS;
							}
							//����
							{
								const auto xradold = this->xradadd;
								this->xradadd = deg2rad(-((this->speed) / (60.f / FPS)) / ((0.1f / 3.6f) / FPS) * 100.f);
								easing_set(&this->xrad, std::clamp(this->xradadd - xradold, deg2rad(-45.f), deg2rad(45.f)), 0.999f);

								auto zradold = this->zradadd;
								this->zradadd = deg2rad(-this->yradadd / (deg2rad(5.f) / FPS) * 30.f);
								easing_set(&this->zrad, std::clamp(this->zradadd - zradold, deg2rad(-15.f), deg2rad(15.f)), 0.995f);

								this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.xvec(), -this->xrad)*MATRIX_ref::RotAxis(this->move.mat.zvec(), this->zrad);
							}
							//
						}
						else {
							this->move.vec.yadd(M_GR / powf(FPS, 2.f));
						}
					}


					this->move.pos += this->move.vec;
					//�ˌ�����
					{
						easing_set(&xrad_shot, deg2rad(-this->Gun_[0].Getfired() * this->Gun_[0].Getcaliber(0) * 50.f) * cos(this->Gun_[0].Getgun_info().yrad), 0.85f);
						easing_set(&zrad_shot, deg2rad(-this->Gun_[0].Getfired() * this->Gun_[0].Getcaliber(0) * 50.f) * sin(this->Gun_[0].Getgun_info().yrad), 0.85f);
						this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.xvec(), -xrad_shot)* MATRIX_ref::RotAxis(this->move.mat.zvec(), zrad_shot);
					}
				}
				//����
				if (this->use_veh.isfloat) {
					this->move.pos.y(std::max(this->move.pos.y(), -this->use_veh.down_in_water));
				}
				//�]��
				this->wheel_Leftadd = -(this->speed * 2.f - this->yradadd * 5.f);
				this->wheel_Rightadd = -(this->speed * 2.f + this->yradadd * 5.f);

				this->wheel_Left += this->wheel_Leftadd;  // -this->yradadd * 5.f;
				this->wheel_Right += this->wheel_Rightadd; // +this->yradadd * 5.f;

				//�ˌ�
				{
					for (int i = 0; i < this->Gun_.size(); i++) {
						auto& cg = this->Gun_[i];
						cg.UpDate(
							key[(i == 0) ? 0 : 1],
							this->obj.frame(cg.Getgun_info().frame3.first),
							(this->obj.frame(cg.Getgun_info().frame3.first) - this->obj.frame(cg.Getgun_info().frame2.first)).Norm(), mine, chara, shared_from_this(), vehicle, hit_obj_p, hit_b_obj_p, MAPPTs);
					}
				}
				//��ԕǔ���
				this->b2mine.UpDate(this->move.vec, this->yradadd);
			}
		}

		void UpDate_after() {
			//��ԉ��Z
			{
				{
					//��ԍ��W���f
					auto pp = this->move.mat.zvec();
					this->move.mat *= MATRIX_ref::RotY(-this->b2mine.Rad() - atan2f(-pp.x(), -pp.z()));
					this->move.pos.x(this->b2mine.Pos().x);
					this->move.pos.z(this->b2mine.Pos().y);
					float spdrec = this->spd_buf;
					easing_set(&this->spd_buf, this->b2mine.Speed() * ((this->spd_buf > 0) ? 1.f : -1.f), 0.99f);
					this->speed = this->spd_buf - spdrec;
					//this->move.mat *= MATRIX_ref::RotAxis(this->move.mat.yvec(),this->yradadd);

					//�]��
					b2Vec2 tmpb2 = b2Vec2((M_GR / FPS) * (this->move.mat.zvec().dot(VGet(0, 1.f, 0))), (M_GR / FPS) * (this->move.mat.yvec().dot(VGet(0, 1.f, 0))));
					int z = 0;
					for (auto& f : foot) {
						size_t i = 0;
						VECTOR_ref vects;
						if (f.Foot.size() != 0) {
							i = 0;
							for (auto& w : this->use_veh.wheelframe) {
								vects = VECTOR_ref(VTransform(VGet(0, 0, 0), MV1GetFrameLocalMatrix(this->obj.get(), w.frame.first)));
								if (vects.x() * ((z == 0) ? 1 : -1) > 0) {
									f.Wheel[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (z == 0) ? this->wheel_Left : this->wheel_Right);
								}
							}
							i = 0;
							for (auto& w : this->use_veh.wheelframe_nospring) {
								vects = VTransform(VGet(0, 0, 0), MV1GetFrameLocalMatrix(this->obj.get(), w.frame.first));
								if (vects.x() * ((z == 0) ? 1 : -1) > 0) {
									f.Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (z == 0) ? this->wheel_Left : this->wheel_Right);
								}
							}
							for (auto& t : f.Foot) {
								t.SetLinearVelocity(tmpb2); //
							}

							f.world->Step(0.3f, 3, 3);

							for (auto& t : f.Foot) {
								t.pos = VGet(t.pos.x(), t.Pos().y, t.Pos().x);
							}
						}
						{
							float rang = (0.0000001f / 3.6f);
							easing_set(&per_sus, std::clamp((rang - (-(this->speed - spd_rec) / FPS)) / rang, -1.f, 1.f), 0.9f);
							i = 0;
							for (auto& w : this->use_veh.b2upsideframe[z]) {
								if (w.second.x() * ((z == 0) ? 1 : -1) > 0) {
									auto& t = f.Foot[i++];
									if (w.second.y() > t.pos.y()) {
										t.pos.y(t.pos.y() * per_sus + w.second.y()*(1.f - per_sus));
									}

									t.pos = VGet(w.second.x(), t.pos.y(), t.pos.z());
									this->obj.SetFrameLocalMatrix(w.first, MATRIX_ref::Mtrans(t.pos));
								}
							}
						}
						z++;
					}
				}

				this->obj.SetMatrix(this->move.mat * MATRIX_ref::Mtrans(this->move.pos));
				this->col.SetMatrix(this->obj.GetMatrix());

				for (auto& g : this->use_veh.b2downsideframe) {
					for (auto& t : g) {
						if (t.colres.HitFlag == TRUE) {
							easing_set(&t.gndsmksize, 0.1f + std::abs(this->speed - spd_rec) / ((0.01f / 3.6f) / FPS) * 1.f, 0.95f);
						}
						else {
							t.gndsmksize = 0.1f;
						}
						t.gndsmkeffcs.handle.SetPos(this->obj.frame(t.frame.first) + this->move.mat.yvec() * (-t.frame.second.y()));
						t.gndsmkeffcs.handle.SetScale(t.gndsmksize);
					}
				}
			}
		}
		//����N��
		const bool set_ref_col(VECTOR_ref&StartPos, VECTOR_ref& EndPos) {
			//���łɋN�����Ă���Ȃ疳��
			if (this->nearhit) {
				return true;
			}
			if (Segment_Point_MinLen(StartPos, EndPos, this->move.pos) <= 5.0f) {
				//����N��
				this->nearhit = true;
				for (int i = 0; i < this->col.mesh_num(); i++) {
					this->col.RefreshCollInfo(-1, i);
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
				for (auto& f : t.Wheel) {
					f.Dispose();
				}
				for (auto& f : t.Yudo) {
					f.Dispose();
				}
			}
			for (auto& g : this->use_veh.b2downsideframe) {
				for (auto& t : g) {
					t.gndsmkeffcs.handle.Dispose();
				}
			}
			this->reset();
		}

		void Draw() {
			MV1SetFrameTextureAddressTransform(this->obj.get(), 0, -this->wheel_Left*0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
			this->obj.DrawMesh(0);
			MV1SetFrameTextureAddressTransform(this->obj.get(), 0, -this->wheel_Right*0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
			this->obj.DrawMesh(1);
			MV1ResetFrameTextureAddressTransform(this->obj.get(), 0);
			for (int i = 2; i < this->obj.mesh_num(); i++) {
				this->obj.DrawMesh(i);
			}
			//col.DrawModel();
		}
		void Draw_ammo() {
			for (auto& cg : this->Gun_) {
				cg.Draw_ammo();
			}
		}
	};
};