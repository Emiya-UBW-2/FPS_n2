#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	//フォントプール
	class FontPool {
		static const int pool_s = 16;
	public:
		class Fonthave {
		public:
			int size = 0;
			FontHandle handle;
		public:
			void Set(int siz_t) {
				this->size = siz_t;
				this->handle = FontHandle::Create(siz_t, DX_FONTTYPE_EDGE);
			}
		};
		std::vector<Fonthave> havehandle;
	public:
		size_t Add(int siz_t) {
			for (auto& h : this->havehandle) {
				if (h.size == siz_t) {
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(siz_t);
			return this->havehandle.size() - 1;
		}

		Fonthave* Get(int siz_t) {
			return &this->havehandle[Add(siz_t)];
		}
	};
	FontPool Fonts;
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
			int first = 0;
			int use_mode = 0;
		public:
			std::string second;
			bool isalways{ false };
			keyhandle* use_handle{ nullptr };
			switchs on_off;

			const auto& Get_first()const noexcept { return first; }

			void Set(int first_t, std::string_view second_t, int mode) {
				first = first_t;
				second = second_t;
				use_mode = mode;
			}
			bool get_key(int id, bool checkupdate) {
				switch (id) {
					//キー
				case 0:
					return CheckHitKey(this->first) != 0;
				case 1:
					if (checkupdate) {
						on_off.GetInput(CheckHitKey(this->first) != 0);
					}
					return on_off.on();
				case 2:
					if (checkupdate) {
						on_off.GetInput(CheckHitKey(this->first) != 0);
					}
					return on_off.trigger();
					//マウス
				case 3:
					return (GetMouseInput() & this->first) != 0;
				case 4:
					if (checkupdate) {
						on_off.GetInput((GetMouseInput() & this->first) != 0);
					}
					return on_off.on();
				case 5:
					if (checkupdate) {
						on_off.GetInput((GetMouseInput() & this->first) != 0);
					}
					return on_off.trigger();
				default:
					return CheckHitKey(this->first) != 0;
				}
			}

			bool get_key_Auto(bool checkupdate) {
				this->isalways = true;
				return this->get_key(this->use_mode, checkupdate);

			}
		};
		class keyhandle {
		public:
			keys key;
			GraphHandle onhandle, offhandle;
			key_pair* use_part{ nullptr };
		};

		FontPool::Fonthave* Font24 = nullptr;

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
			return key_use_ID[id_t].get_key_Auto(true);
		}
		bool get_mouse_use(int id_t) {
			return mouse_use_ID[id_t].get_key_Auto(true);
		}
		//
		key_bind(void) noexcept {
			SetUseASyncLoadFlag(FALSE);
			Font24 = Fonts.Get(y_r(24));
			mousehandle = GraphHandle::Load("data/key/mouse.png");
			SetTransColor(0, 255, 0);
			keyboad = GraphHandle::Load("data/key/keyboad.png");
			SetTransColor(0, 0, 0);
			//
			{
				key_pair tmp_k;
				tmp_k.Set(KEY_INPUT_W, "前進", 0);
				this->key_use_ID.emplace_back(tmp_k);//0
				tmp_k.Set(KEY_INPUT_S, "後退", 0);
				this->key_use_ID.emplace_back(tmp_k);//1
				tmp_k.Set(KEY_INPUT_D, "右歩き", 0);
				this->key_use_ID.emplace_back(tmp_k);//2
				tmp_k.Set(KEY_INPUT_A, "左歩き", 0);
				this->key_use_ID.emplace_back(tmp_k);//3
				tmp_k.Set(KEY_INPUT_Q, "左リーン", 0);
				this->key_use_ID.emplace_back(tmp_k);//4
				tmp_k.Set(KEY_INPUT_E, "右リーン", 0);
				this->key_use_ID.emplace_back(tmp_k);//5
				tmp_k.Set(KEY_INPUT_R, "リロード", 0);
				this->key_use_ID.emplace_back(tmp_k);//6
				tmp_k.Set(KEY_INPUT_F, "アイテム取得", 2);
				this->key_use_ID.emplace_back(tmp_k);//7
				tmp_k.Set(KEY_INPUT_G, "グレネード投擲", 2);
				this->key_use_ID.emplace_back(tmp_k);//8
				tmp_k.Set(KEY_INPUT_C, "しゃがみ", 1);
				this->key_use_ID.emplace_back(tmp_k);//9
				tmp_k.Set(KEY_INPUT_O, "タイトル画面に戻る", 1);
				this->key_use_ID.emplace_back(tmp_k);//10
				tmp_k.Set(KEY_INPUT_ESCAPE, "強制終了", 1);
				this->key_use_ID.emplace_back(tmp_k);//11
				tmp_k.Set(KEY_INPUT_Z, "マガジン整理", 2);
				this->key_use_ID.emplace_back(tmp_k);//12
				tmp_k.Set(KEY_INPUT_LSHIFT, "走る", 0);
				this->key_use_ID.emplace_back(tmp_k);//13
				tmp_k.Set(KEY_INPUT_SPACE, "ジャンプ", 2);
				this->key_use_ID.emplace_back(tmp_k);//14
				tmp_k.Set(KEY_INPUT_LCONTROL, "視点切替", 0);
				this->key_use_ID.emplace_back(tmp_k);//15
				tmp_k.Set(KEY_INPUT_F1, "キー案内", 1);
				this->key_use_ID.emplace_back(tmp_k);//16
				tmp_k.Set(KEY_INPUT_V, "眺める", 2);
				this->key_use_ID.emplace_back(tmp_k);//17
				tmp_k.Set(KEY_INPUT_P, "ポーズ", 1);
				this->key_use_ID.emplace_back(tmp_k);//18
				tmp_k.Set(KEY_INPUT_H, "治療キット排出", 2);
				this->key_use_ID.emplace_back(tmp_k);//19
				tmp_k.Set(KEY_INPUT_B, "乗車", 2);
				this->key_use_ID.emplace_back(tmp_k);//20
				//
				tmp_k.Set(MOUSE_INPUT_LEFT, "射撃", 3);
				this->mouse_use_ID.emplace_back(tmp_k);//0
				tmp_k.Set(MOUSE_INPUT_MIDDLE, "セレクター切替", 5);
				this->mouse_use_ID.emplace_back(tmp_k);//1
				tmp_k.Set(MOUSE_INPUT_RIGHT, "エイム", 3);
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
							if (i.Get_first() == m.key.mac) {
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
							if (i.Get_first() == m.key.mac) {
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
		const auto Esc_key(void) noexcept { return this->key_use_ID[11].get_key_Auto(true); }
		//
		void reSet_isalways(void) noexcept {
			for (auto& i : this->key_use_ID) {
				i.isalways = false;
			}
			for (auto& i : this->mouse_use_ID) {
				i.isalways = false;
			}
		}
		//
		void draw(void) noexcept {
			auto tmp_f1 = this->key_use_ID[16].get_key_Auto(true);
			easing_set(&F1_f, float(tmp_f1), 0.9f);
			noF1_f = std::max(noF1_f - 1.f / FPS, 0.f);
			//インフォ
			if (F1_f > 0.1f) {
				int xp_t = y_r(100), yp_t = y_r(300);
				int xp_sk = xp_t, yp_sk = yp_t, y_size_k = y_r(48);
				int xp_s = y_r(1500), yp_s = y_r(200), y_size = y_r(32);
				//背景
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * F1_f));
				DrawBox(0, 0, deskx, desky, GetColor(0, 0, 0), TRUE);
				if (F1_f > 0.9f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					keyboad.DrawExtendGraph(0, 0, deskx, desky, true);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//前面
				if (F1_f > 0.9f) {
					//キーボード＋マウス全部
					{
						for (auto& m : this->keyg) {
							if (m.use_part != nullptr) {
								xp_sk = xp_t + y_r(m.key.px);
								yp_sk = yp_t + y_r(m.key.py);
								if (m.use_part->get_key(0, false)) {//keyboad
									m.onhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - y_r(4)) / 26.f, 0.f, false);
								}
								else {
									m.offhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - y_r(4)) / 26.f, 0.f, false);
								}
							}
						}
						int yss = 0;
						xp_sk = y_r(1100);
						yp_sk = y_r(800);
						for (auto& m : this->keyg2) {
							if (m.use_part != nullptr) {
								if (m.use_part->get_key(3, false)) {
									m.onhandle.GetSize(nullptr, &yss);
									m.onhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_r(float(256) / yss * 100)) / 100.f, 0.f, true);
								}
								else {
									m.offhandle.GetSize(nullptr, &yss);
									m.offhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_r(float(256) / yss * 100)) / 100.f, 0.f, true);
								}
							}
						}
					}
					//詳細
					{
						int xss = 0, yss = 0;
						float siz_t = float(y_size - 4) / 25.f;
						for (auto& i : this->key_use_ID) {
							if (i.isalways && i.use_handle != nullptr) {
								if (i.get_key(0, false)) {
									i.use_handle->onhandle.GetSize(&xss, &yss);
									xss = int(float(xss) * siz_t);
									yss = int(float(yss) * siz_t);
									i.use_handle->onhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, siz_t, 0.f, false);
								}
								else {
									i.use_handle->offhandle.GetSize(&xss, &yss);
									xss = int(float(xss) * siz_t);
									yss = int(float(yss) * siz_t);
									i.use_handle->offhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, siz_t, 0.f, false);
								}
								Font24->handle.DrawString(xp_s, yp_s + (y_size - Font24->size) / 2, i.second, GetColor(255, 255, 255)); yp_s += y_size;
							}
						}
						for (auto& i : this->mouse_use_ID) {
							if (i.isalways && i.use_handle != nullptr) {
								{
									mousehandle.GetSize(nullptr, &yss);
									mousehandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								if (i.get_key(3, false)) {
									i.use_handle->onhandle.GetSize(nullptr, &yss);
									i.use_handle->onhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								else {
									i.use_handle->offhandle.GetSize(nullptr, &yss);
									i.use_handle->offhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								Font24->handle.DrawString(xp_s, yp_s + (y_size - Font24->size) / 2, i.second, GetColor(255, 255, 255)); yp_s += y_size;
							}
						}
					}
				}
			}
			//常時表示
			if (!tmp_f1) {
				if (noF1_f >= 0.1f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * std::clamp(noF1_f, 0.f, 1.f)));
					int xp_s = deskx - y_r(700), yp_s = desky - y_r(28), x_size = y_r(26), y_size = y_r(24);
					int xss = 0, yss = 0;
					float siz_t = float(y_size - 4) / 25.f;
					for (auto& i : this->key_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg) {
								if (m.key.mac == i.Get_first()) {
									if (i.get_key(0, false)) {
										noF1_f = 3.f;
										m.onhandle.GetSize(&xss, &yss);
										xss = int(float(xss) * siz_t);
										yss = int(float(yss) * siz_t);
										m.onhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, siz_t, 0.f, false);
									}
									else {
										m.offhandle.GetSize(&xss, &yss);
										xss = int(float(xss) * siz_t);
										yss = int(float(yss) * siz_t);
										m.offhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, siz_t, 0.f, false);
									}
									xp_s += (x_size - y_size + xss);
								}
							}
						}
					}
					for (auto& i : this->mouse_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg2) {
								if (m.key.mac == i.Get_first()) {
									{
										mousehandle.GetSize(&xss, &yss);
										mousehandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
									}
									if (i.get_key(3, false)) {
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
								if (m.key.mac == i.Get_first()) {
									if (i.get_key(0, false)) {
										noF1_f = 3.f;
									}
								}
							}
						}
					}
					for (auto& i : this->mouse_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg2) {
								if (m.key.mac == i.Get_first()) {
									if (i.get_key(3, false)) {
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
		//
	};
	//キーバインド
	class pause_menu {
	private:
		std::shared_ptr<key_bind> KeyBind{ nullptr };

		FontPool::Fonthave* Font24 = nullptr;

		float P_f = 0.0f;
	public:
		//
		pause_menu(std::shared_ptr<key_bind>& KeyBind_t) noexcept {
			KeyBind = KeyBind_t;
			SetUseASyncLoadFlag(FALSE);
			Font24 = Fonts.Get(y_r(24));
		}
		//
		const auto Pause_key(void) noexcept { return KeyBind->key_use_ID[18].get_key_Auto(true); }
		//
		bool Update(void) noexcept {
			KeyBind->key_use_ID[10].isalways = KeyBind->key_use_ID[18].get_key_Auto(false);

			SetMouseDispFlag(TRUE);

			bool selend = true;
			//強制帰還はポーズメニューで
			if (KeyBind->key_use_ID[10].get_key(0, false)) {
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
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * P_f));
				DrawBox(0, 0, deskx, desky, GetColor(0, 0, 0), TRUE);
				if (P_f > 0.9f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					//背景画像
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//前面
				if (P_f > 0.9f) {
					yp_t = 100;
					//
					Font24->handle.DrawString_RIGHT(deskx - 100, yp_t, "オプション", GetColor(0, 255, 0)); yp_t += Font24->size + 30;
					//
					Font24->handle.DrawString_RIGHT(deskx - 100, yp_t, "Pキーで戦闘に戻る", GetColor(0, 255, 0)); yp_t += Font24->size + 30;
					//
					Font24->handle.DrawString_RIGHT(deskx - 100, yp_t, "Oキーで強制帰還", GetColor(0, 255, 0)); yp_t += Font24->size + 30;
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
		EnumGunParts type_ = EnumGunParts::PARTS_NONE;			//パーツの種類
		EnumAttachPoint pt_cat_ = EnumAttachPoint::POINTS_NONE;	//ベースパーツの場所
		EnumGunParts pt_type_ = EnumGunParts::PARTS_NONE;		//ベースパーツの種類
		size_t pt_sel_ = 0;						//ベースパーツの番号(マウントなど)
	};
	//
	class shaders {
		int pshandle{ -1 }, vshandle{ -1 };
		int pscbhandle{ -1 };
		int pscbhandle2{ -1 };
	public:
		void Init(std::string vs, std::string ps) {
			this->vshandle = LoadVertexShader(("shader/" + vs).c_str());	// 頂点シェーダーバイナリコードの読み込み
			this->pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pshandle = LoadPixelShader(("shader/" + ps).c_str());		// ピクセルシェーダーバイナリコードの読み込み
		}
		void set_dispsize(int dispx, int dispy) {
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->pscbhandle);			// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = float(dispx);
			dispsize->v = float(dispy);
			UpdateShaderConstantBuffer(this->pscbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		void set_param(float param1, float param2, float param3, float param4) {
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->pscbhandle2);			// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->pscbhandle2);							// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		void draw(VERTEX3DSHADER Screen_vertex[]) {
			draw_lamda([&] {DrawPolygon3DToShader(Screen_vertex, 2);});
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
					this->magazine_ammo_f[0].Set_World(i, this->model);
				}
				if (p == std::string("ammo2")) {
					this->magazine_ammo_f[1].Set_World(i, this->model);
				}
				if (p == std::string("LEFT_mag")) {
					this->LEFT_mag_frame[0].Set_World(i, this->model);
					this->LEFT_mag_frame[1].Set_Local(i + 1, this->model);
					this->LEFT_mag_frame[2].Set_Local(i + 2, this->model);
				}
				if (p == std::string("magazine_fall")) {
					this->magazine_f[0].Set_World(i, this->model);
					this->magazine_f[1].Set_Local(i + 1, this->model);
				}
			}
			this->mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			doing();
			FileRead_close(mdata);
		}
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
			shot = SoundHandle::Load(shot_path);
			slide_path = "data/audio/chara/slide_" + getparams::_str(mdata) + ".wav";
			slide = SoundHandle::Load(slide_path);
			trigger_path = "data/audio/chara/trigger_" + getparams::_str(mdata) + ".wav";
			trigger = SoundHandle::Load(trigger_path);
			magazine_down = SoundHandle::Load("data/audio/chara/mag_down_" + getparams::_str(mdata) + ".wav");
			magazine_Set = SoundHandle::Load("data/audio/chara/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/chara/case_2.wav");
			load_ = SoundHandle::Load("data/audio/chara/load.wav");
			sort_magazine = SoundHandle::Load("data/audio/chara/sort.wav");
			foot_ = SoundHandle::Load("data/audio/chara/foot_sand.wav");
			explosion = SoundHandle::Load("data/audio/chara/explosion.wav");

			voice_damage = SoundHandle::Load("data/audio/voice/damage.wav");
			voice_death = SoundHandle::Load("data/audio/voice/death.wav");
			voice_breath = SoundHandle::Load("data/audio/voice/breath.wav");
			voice_breath_run = SoundHandle::Load("data/audio/voice/breath_run.wav");

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
			this->case_down = tgt.case_down.Duplicate();
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
			this->caliber = getparams::_float(mdata) * 0.001f;//口径
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
	//インスタシング
	class Model_Instance {
	public:
		int hitss = 0;					/*hitsの数*/
		std::vector<VERTEX3D> hitsver;	/*hits*/
		std::vector<DWORD> hitsind;	    /*hits*/
		int VerBuf = -1, IndexBuf = -1;	/*hits*/
		MV1 hits;						/*hitsモデル*/
		GraphHandle hits_pic;			/*画像ハンドル*/
		int IndexNum = -1, VerNum = -1;	/*hits*/
		int vnum = -1, pnum = -1;		/*hits*/
		MV1_REF_POLYGONLIST RefMesh;	/*hits*/
		//初期化
		void Init(std::string pngpath, std::string mv1path) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->hits_pic = GraphHandle::Load(pngpath);		 /*grass*/
			MV1::Load(mv1path, &this->hits, false);	//弾痕
			Init_one();
		}
		void Init_one() {
			MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
			this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
		}
		//毎回のリセット
		void Clear(void) noexcept {
			this->hitss = 0;
			this->vnum = 0;
			this->pnum = 0;
			this->hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			this->hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			this->hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			this->hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
		}

		void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->hitss++;
			Set_start();
			{
				float asize = 200.f * caliber;
				const auto& y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				this->hits.SetMatrix(mat * MATRIX_ref::Mtrans(pos));
			}
			Set_one();
		}
		void Set_start() {
			this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				/*インデックスの数を取得*/
			this->VerNum = this->RefMesh.VertexNum * this->hitss;						/*頂点の数を取得*/
			this->hitsver.resize(this->VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			this->hitsind.resize(this->IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
		}
		void Set_one() {
			Init_one();
			for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
				auto& g = this->hitsver[j + this->vnum];
				g.pos = this->RefMesh.Vertexs[j].Position;
				g.norm = this->RefMesh.Vertexs[j].Normal;
				g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
				g.spc = this->RefMesh.Vertexs[j].SpecularColor;
				g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
					this->hitsind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
			}
			this->vnum += this->RefMesh.VertexNum;
			this->pnum += this->RefMesh.PolygonNum * 3;
		}

		void update(void) noexcept {
			this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
			this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, this->hitsver.data(), this->VerNum, this->VerBuf);
			SetIndexBufferData(0, this->hitsind.data(), this->IndexNum, this->IndexBuf);
		}
		void draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			{
				DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
			}
			//SetDrawAlphaTest(-1, 0);
		}
	};
	//命中根
	class HIT_PASSIVE {
		//雲
		Model_Instance inst;
		bool isUPDate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			inst.Init("data/model/hit/hit.png", "data/model/hit/model.mv1");
		}
		//毎回のリセット
		void Clear(void) noexcept {
			inst.Clear();
		}

		void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			inst.Set(caliber, Position, Normal, Zvec);
			isUPDate = true;
		}
		void update(void) noexcept {
			if (isUPDate) {
				isUPDate = false;
				inst.update();
			}
		}
		void draw(void) noexcept {
			inst.draw();
		}
	};
	class HIT_BLOOD_PASSIVE {
		//雲
		Model_Instance inst;
		bool isUPDate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			inst.Init("data/model/hit_blood/hit.png", "data/model/hit_blood/model.mv1");
		}
		//毎回のリセット
		void Clear(void) noexcept {
			inst.Clear();
		}

		void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			inst.Set(caliber, Position, Normal, Zvec);
			isUPDate = true;
		}
		void update(void) noexcept {
			if (isUPDate) {
				isUPDate = false;
				inst.update();
			}
		}
		void draw(void) noexcept {
			inst.draw();
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
		EnumGunParts type = EnumGunParts::PARTS_NONE;
	public:
		float recoil_xup = 0.f;				//反動
		float recoil_xdn = 0.f;				//反動
		float recoil_yup = 0.f;				//反動
		float recoil_ydn = 0.f;				//反動
		float reload_time = 1.f;			//再装填時間
		std::vector <EnumSELECTER> select;			//セレクター
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
		void Set_type(EnumGunParts type_t) { this->type = type_t; }
		//
		static void Set_Pre(std::vector<GUNPARTs>* data, std::string file_name, EnumGunParts type_t = EnumGunParts::PARTS_NONE) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
						data->back().Set_type(type_t);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
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
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "semi") {
							this->select.emplace_back(EnumSELECTER::SELECT_SEMI);	//セミオート=1
						}
						else if (getparams::getright(p.c_str()) == "full") {
							this->select.emplace_back(EnumSELECTER::SELECT_FULL);	//フルオート=2
						}
						else if (getparams::getright(p.c_str()) == "3b") {
							this->select.emplace_back(EnumSELECTER::SELECT_3B);	//3連バースト=3
						}
						else if (getparams::getright(p.c_str()) == "2b") {
							this->select.emplace_back(EnumSELECTER::SELECT_2B);	//2連バースト=4
						}
						else {
							this->select.emplace_back(EnumSELECTER::SELECT_SEMI);
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
					this->mag_cnt = getparams::_long(this->mod.mdata);			//弾数
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				if (this->type == EnumGunParts::PARTS_LAM) {
					//レーザーかライトか
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "laser") {
							this->select_lam.emplace_back(EnumSELECT_LAM::SELECTLAM_LASER);	//レーザー
						}
						else if (getparams::getright(p.c_str()) == "light") {
							this->select_lam.emplace_back(EnumSELECT_LAM::SELECTLAM_LIGHT);	//ライト
						}
						else {
							this->select_lam.emplace_back(EnumSELECT_LAM::SELECTLAM_LASER);	//レーザー
						}
					}
				}
				if (this->type == EnumGunParts::PARTS_SIGHT) {
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
		int Select_Chose(EnumSELECTER sel_chose) {
			auto ans = std::find(this->select.begin(), this->select.end(), sel_chose);
			return int((ans != this->select.end()) ? (ans - this->select.begin()) : -1);
		}
		void Set_gun_select(std::vector<MV1::ani*>& gunanime_sel, EnumSELECTER selecting) {
			for (auto& sel : this->select) {
				easing_set(&gunanime_sel[&sel - &this->select[0]]->per, float(int(sel == this->select[(int)selecting])), 0.5f);
			}
		}
	};
	//薬品データ
	class Meds :public BASE_Obj {
	public:
		int repair = 0;
		static void Set_Pre(std::vector<Meds>* data, std::string file_name) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
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
		static void Set_Pre(std::vector<Grenades>* data, std::string file_name) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
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
		void Set_item(GUNPARTs* magdata, const moves& move_, size_t dnm = SIZE_MAX) {
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
		void Set_item(Meds* meddata, const moves& move_) {
			this->move = move_;
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.get_model().Duplicate();
		}
		//med
		void Set_item(Grenades* gredata, const moves& move_) {
			this->move = move_;
			this->ptr_gre = gredata;
			this->obj = this->ptr_gre->mod.get_model().Duplicate();
			this->del_timer = this->ptr_gre->time;
		}
	public:
		//mag
		Items(size_t id, GUNPARTs* magdata, const moves& move_, size_t dnm = SIZE_MAX) {
			this->id_t = id;
			Set_item(magdata, move_, dnm);
		}
		bool Set_item_(GUNPARTs* magdata, const moves& move_, size_t dnm) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(magdata, move_, dnm);
				return true;
			}
			return false;
		}
		//med
		Items(size_t id, Meds* meddata, const moves& move_) {
			this->id_t = id;
			this->Set_item(meddata, move_);
		}
		bool Set_item_(Meds* meddata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(meddata, move_);
				return true;
			}
			return false;
		}
		//gre
		Items(size_t id, Grenades* gredata, const moves& move_) {
			this->id_t = id;
			this->Set_item(gredata, move_);
		}
		bool Set_item_(Grenades* gredata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(gredata, move_);
				return true;
			}
			return false;
		}
		//
		void UpDate(std::vector<std::shared_ptr<Items>>& item, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) {
			auto old = this->move.pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
				this->obj.SetMatrix(this->move.mat * MATRIX_ref::Mtrans(this->move.pos));
				this->move.Update_Physics();
				for (auto& p : item) {
					if ((p->ptr_mag != nullptr || p->ptr_med != nullptr || p->ptr_gre != nullptr) && p.get() != this) {
						if ((p->move.pos - this->move.pos).size() <= 0.35f) {
							p->move.vec.xadd((p->move.pos - this->move.pos).x() * 5.f / FPS);
							p->move.vec.zadd((p->move.pos - this->move.pos).z() * 5.f / FPS);
							this->move.vec.xadd((this->move.pos - p->move.pos).x() * 5.f / FPS);
							this->move.vec.zadd((this->move.pos - p->move.pos).z() * 5.f / FPS);
						}
					}
				}
				if (this->ptr_gre != nullptr) {
					auto ColResGround = map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (ColResGround.HitFlag == TRUE) {
						this->move.HitGround(ColResGround, 0.005f);
						auto fvec = this->move.vec.Norm();
						auto nvec = VECTOR_ref(ColResGround.Normal).Norm();
						this->move.vec = (fvec + nvec * ((fvec * -1.f).dot(nvec) * 2.f)) * (this->move.vec.size() * 0.5f);
					}
				}
				else {
					auto ColResGround = map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (ColResGround.HitFlag == TRUE) {
						this->move.HitGround(ColResGround, 0.005f);
						this->move.vec.clear();
						//easing_set(&this->move.vec, VECTOR_ref::zero(), 0.8f);
					}
				}
				//
			}
			//
			if (this->ptr_mag != nullptr || this->ptr_gre != nullptr) {
				this->del_timer -= 1.f / FPS;
			}
		}
		template<class PLAYER_CHARA>
		void Get_item_2(VECTOR_ref StartPos, VECTOR_ref EndPos, const std::shared_ptr<PLAYER_CHARA>& chara, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) {
			if (this->flag_canlook_player) {
				bool zz = false;
				if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
					auto ColResGround = map_col_line(StartPos, EndPos);
					if (ColResGround.HitFlag == TRUE) {
						EndPos = ColResGround.HitPosition;
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
							chara->Damage.AddHP(this->ptr_med->repair);
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
		void Check_CameraViewClip(bool use_occlusion, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) noexcept {
			this->flag_canlook_player = true;
			auto ttt = this->move.pos;
			if (CheckCameraViewClip_Box((ttt + VECTOR_ref::vget(-0.3f, 0, -0.3f)).get(), (ttt + VECTOR_ref::vget(0.3f, 0.3f, 0.3f)).get())) {
				this->flag_canlook_player = false;
				return;
			}
			if (use_occlusion) {
				if (map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.3f, 0)).HitFlag == TRUE &&
					map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.0f, 0)).HitFlag == TRUE) {
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
		template<class PLAYER_CHARA>
		bool Detach_gre(std::shared_ptr<PLAYER_CHARA>& killer, std::vector<std::shared_ptr<PLAYER_CHARA>>& chara, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) noexcept {
			if (this->ptr_gre != nullptr && this->del_timer <= 0.f) {
				//effect
				killer->Set_eff(Effect::ef_greexp, this->move.pos, VECTOR_ref::front(), 0.1f / 0.1f);
				//
				killer->get_audio().explosion.vol(255);
				killer->get_audio().explosion.play_3D(this->move.pos, 100.f);
				//グレ爆破
				this->Detach_item();
				for (auto& tgt : chara) {
					tgt->calc_gredamage(killer);
					if (tgt->Damage.get_alive()) {
						float scale = (this->move.pos - tgt->get_head_pos()).size();
						if (scale < 10.f) {
							if (!(map_col_line(this->move.pos, tgt->get_head_pos()).HitFlag == TRUE)) {
								int damage = int(150.f * (10.f - scale) / 10.f);
								damage = std::clamp(damage, 0, 100);

								float x_1 = sinf(tgt->get_body_yrad());
								float y_1 = cosf(tgt->get_body_yrad());
								auto vc = (killer->get_pos() - tgt->get_pos()).Norm();
								tgt->Damage.SubHP(damage, atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z()));
								tgt->Damage.SubHP_Parts(damage, 0);
								if (!tgt->Damage.get_alive()) {
									killer->scores.set_kill(&tgt - &chara.front(), 70);
									tgt->scores.set_death(&killer - &chara.front());
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
	//戦車砲データ
	class gun_frame {
		int type = 0;
		int rounds = 0;
		std::string name;
		std::vector<std::string> useammo;
		std::vector<Ammos> Spec;	/**/
		float load_time = 0.f;
		frames frame1;
		frames frame2;
		frames frame3;
	public:

		const auto& get_rounds()const noexcept { return rounds; }
		const auto& get_Spec()const noexcept { return Spec; }

		const auto& get_load_time()const noexcept { return load_time; }
		const auto& get_frame1()const noexcept { return frame1; }
		const auto& get_frame2()const noexcept { return frame2; }
		const auto& get_frame3()const noexcept { return frame3; }
		//const auto& get_()const noexcept { return; }

		gun_frame() {
			frame1.first = -1;
			frame2.first = -1;
			frame3.first = -1;
		}

		void Set(const MV1& obj, int i) {
			this->frame1.Set_World(i, obj);
			this->frame3.first = -1;
			this->frame2.first = -1;
			auto p2 = obj.frame_parent(this->frame1.first);
			if (p2 >= 0) {
				this->frame1.second -= obj.frame(int(p2)); //親がいる時引いとく
			}
			if (obj.frame_child_num(this->frame1.first) <= 0) {
				return;
			}
			int child_num = (int)obj.frame_child(this->frame1.first, 0);
			if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
				this->frame2.Set_Local(child_num, obj);
			}
			if (this->frame2.first == -1) {
				return;
			}
			if (obj.frame_child_num(this->frame2.first) <= 0) {
				return;
			}
			{
				this->frame3.Set_Local((int)obj.frame_child(this->frame2.first, 0), obj);
			}
		}

		void Set_performance(int mdata, const std::string& stt) {
			this->name = getparams::getright(stt);
			this->load_time = getparams::_float(mdata);
			this->rounds = getparams::_int(mdata);

			this->Spec.resize(this->Spec.size() + 1);
			this->Spec.back().Set_before("data/ammo/", getparams::_str(mdata));
			while (true) {
				auto stp = getparams::get_str(mdata);
				if (stp.find("useammo" + std::to_string(this->useammo.size())) == std::string::npos) {
					break;
				}
				this->useammo.emplace_back(getparams::getright(stp));
			}
		}

		void Set_Ammos_after() {
			for (auto& a : this->Spec) {
				a.Set();
			}
		}
	};
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
	//戦車データ
	class Vehcs {
		std::string name;									/**/
		MV1 obj, col;										/**/
		VECTOR_ref minpos, maxpos;							/**/
		std::vector<gun_frame> gunframe;					/**/
		std::vector<frames> wheelframe;						/**/
		std::vector<frames> wheelframe_nospring;			/*誘導輪回転*/
		int HP = 0;											/**/
		std::vector<std::pair<int, float>> armer_mesh;		/*装甲ID*/
		std::vector<int> space_mesh;						/*装甲ID*/
		std::vector<int> module_mesh;						/*装甲ID*/
		bool isfloat{ false };								/*浮くかどうか*/
		float down_in_water = 0.f;							/*沈む判定箇所*/
		float flont_speed_limit = 0.f;						/*前進速度(km/h)*/
		float back_speed_limit = 0.f;						/*後退速度(km/h)*/
		float body_rad_limit = 0.f;							/*旋回速度(度/秒)*/
		float turret_rad_limit = 0.f;						/*砲塔駆動速度(度/秒)*/
		frames fps_view;									//コックピット
		GraphHandle ui_pic;									//シルエット
		int pic_x, pic_y;									//サイズ
		std::array<int, 4> square{ 0 };						//車輛の四辺
		std::array<std::vector<frames>, 2> b2upsideframe;	/*履帯上*/
		std::array<std::vector<frames>, 2> b2downsideframe;	/*履帯*/


		void copy(const Vehcs& t) {
			this->wheelframe.clear();
			for (auto& p : t.wheelframe) {
				this->wheelframe.resize(this->wheelframe.size() + 1);
				this->wheelframe.back() = p;
			}
			this->wheelframe_nospring.clear();
			for (auto& p : t.wheelframe_nospring) {
				this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
				this->wheelframe_nospring.back() = p;
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
		const auto& Get_name()const noexcept { return name; }

		const auto& Get_obj()const noexcept { return obj; }
		const auto& Get_col()const noexcept { return col; }
		const auto& Get_minpos()const noexcept { return minpos; }
		const auto& Get_maxpos()const noexcept { return maxpos; }
		const auto& Get_gunframe(size_t ID_t)const noexcept { return gunframe[ID_t]; }
		const auto& Get_gunframe()const noexcept { return gunframe; }
		const auto& Get_wheelframe()const noexcept { return wheelframe; }
		const auto& Get_wheelframe_nospring()const noexcept { return wheelframe_nospring; }
		const auto& Get_HP()const noexcept { return HP; }
		const auto& Get_armer_mesh()const noexcept { return armer_mesh; }
		const auto& Get_space_mesh()const noexcept { return space_mesh; }
		const auto& Get_module_mesh()const noexcept { return module_mesh; }
		const auto& Get_isfloat()const noexcept { return isfloat; }
		const auto& Get_down_in_water()const noexcept { return down_in_water; }
		const auto& Get_flont_speed_limit()const noexcept { return flont_speed_limit; }
		const auto& Get_back_speed_limit()const noexcept { return back_speed_limit; }
		const auto& Get_body_rad_limit()const noexcept { return body_rad_limit; }
		const auto& Get_turret_rad_limit()const noexcept { return turret_rad_limit; }
		const auto& Get_fps_view()const noexcept { return fps_view; }
		const auto& Get_ui_pic()const noexcept { return ui_pic; }
		const auto& Get_pic_x()const noexcept { return pic_x; }
		const auto& Get_pic_y()const noexcept { return pic_y; }
		const auto& Get_square(size_t ID_t)const noexcept { return square[ID_t]; }
		const auto& Get_square()const noexcept { return square; }
		const auto& Get_b2upsideframe(size_t ID_t)const noexcept { return b2upsideframe[ID_t]; }
		const auto& Get_b2upsideframe()const noexcept { return b2upsideframe; }
		const auto& Get_b2downsideframe()const noexcept { return b2downsideframe; }
		//コンストラクタ
		Vehcs() { }
		Vehcs(const Vehcs& t) {
			this->copy(t);
		}
		void operator=(const Vehcs& t) {
			this->copy(t);
		}
		//事前読み込み
		static void Set_Pre(std::vector<Vehcs>* veh_, const char* name) {
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
				MV1::Load(std::string(name) + t.name + "/model.mv1", &t.obj, true);
				MV1::Load(std::string(name) + t.name + "/col.mv1", &t.col, true);
				t.ui_pic = GraphHandle::Load(std::string(name) + t.name + "/pic.png");
			}
		}
		//メイン読み込み
		void Set() {
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
					this->wheelframe.back().Set_World(i, this->obj);
				}
				else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
					this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
					this->wheelframe_nospring.back().Set_World(i, this->obj);
				}
				else if (p.find("旋回", 0) != std::string::npos) {
					this->gunframe.resize(this->gunframe.size() + 1);
					this->gunframe.back().Set(this->obj, i);
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
							this->b2upsideframe[0].back().Set_World(i + 1 + z, this->obj);
						}
						else {
							this->b2upsideframe[1].resize(this->b2upsideframe[1].size() + 1);
							this->b2upsideframe[1].back().Set_World(i + 1 + z, this->obj);
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
							this->b2downsideframe[0].back().Set_World(i + 1 + z, this->obj);
						}
						else {
							this->b2downsideframe[1].resize(this->b2downsideframe[1].size() + 1);
							this->b2downsideframe[1].back().Set_World(i + 1 + z, this->obj);
						}
					}
				}

			}
			//2	左後部0
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (f.second.x() >= 0) {
						this->square[0] = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[0] != f.first) {
						if (f.second.x() >= 0) {
							if (tmp < f.second.z()) {
								this->square[0] = f.first;
								tmp = f.second.z();
							}
						}
					}
				}
			}
			//10	左前部1
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (f.second.x() >= 0) {
						this->square[1] = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[1] != f.first) {
						if (f.second.x() >= 0) {
							if (tmp > f.second.z()) {
								this->square[1] = f.first;
								tmp = f.second.z();
							}
						}
					}
				}
			}
			//3	右後部2
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (!(f.second.x() >= 0)) {
						this->square[2] = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[2] != f.first) {
						if (!(f.second.x() >= 0)) {
							if (tmp < f.second.z()) {
								this->square[2] = f.first;
								tmp = f.second.z();
							}
						}
					}
				}
			}
			//11	右前部3
			{
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if (!(f.second.x() >= 0)) {
						this->square[3] = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (this->square[3] != f.first) {
						if (!(f.second.x() >= 0)) {
							if (tmp > f.second.z()) {
								this->square[3] = f.first;
								tmp = f.second.z();
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
				this->turret_rad_limit = deg2rad(getparams::_float(mdata));
				this->HP = getparams::_int(mdata);
				auto stt = getparams::get_str(mdata);
				for (auto& g : this->gunframe) { g.Set_performance(mdata, stt); }
				this->audio.Set(mdata);						//サウンド
				FileRead_close(mdata);
				for (auto& g : this->gunframe) { g.Set_Ammos_after(); }
			}
		}
	};

	//パーツデータ管理
	class GUNPARTS_Control {
	private:
		std::vector<GUNPARTs> magazine_data;		//GUNデータ
		std::vector<GUNPARTs> mazzule_data;			//GUNデータ
		std::vector<GUNPARTs> grip_data;			//GUNデータ
		std::vector<GUNPARTs> uperhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> underhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> stock_data;			//GUNデータ
		std::vector<GUNPARTs> dustcover_data;		//GUNデータ
		std::vector<GUNPARTs> sight_data;			//GUNデータ
		std::vector<GUNPARTs> foregrip_data;		//GUNデータ
		std::vector<GUNPARTs> lam_data;				//GUNデータ
		std::vector<GUNPARTs> mount_base_data;		//GUNデータ
		std::vector<GUNPARTs> mount_data;			//GUNデータ
		std::vector<GUNPARTs> gun_data;				//GUNデータ
	public:
		std::vector<GUNPARTs>* Get_Parts_Data_Ptr(EnumGunParts type_sel) noexcept {
			switch (type_sel) {
			case EnumGunParts::PARTS_MAGAZINE:
				return &this->magazine_data;
			case EnumGunParts::PARTS_MAZZULE:
				return &this->mazzule_data;
			case EnumGunParts::PARTS_GRIP:
				return &this->grip_data;
			case EnumGunParts::PARTS_UPER_HGUARD:
				return &this->uperhandguard_data;
			case EnumGunParts::PARTS_UNDER_HGUARD:
				return &this->underhandguard_data;
			case EnumGunParts::PARTS_DUSTCOVER:
				return &this->dustcover_data;
			case EnumGunParts::PARTS_STOCK:
				return &this->stock_data;
			case EnumGunParts::PARTS_LAM:
				return &this->lam_data;
			case EnumGunParts::PARTS_MOUNT_BASE:
				return &this->mount_base_data;
			case EnumGunParts::PARTS_MOUNT:
				return &this->mount_data;
			case EnumGunParts::PARTS_SIGHT:
				return &this->sight_data;
			case EnumGunParts::PARTS_FOREGRIP:
				return &this->foregrip_data;
			case EnumGunParts::PARTS_GUN:
				return &this->gun_data;
			default:
				/*
				PARTS_NONE,
				PARTS_BASE,
				*/
				return nullptr;
			}
		}
		std::vector<GUNPARTs>& Get_Parts_Data(EnumGunParts type_sel) noexcept {
			auto ptr = Get_Parts_Data_Ptr(type_sel);
			return *ptr;
		}
		void Set_Pre() {
			GUNPARTs::Set_Pre(&this->grip_data, "data/Guns/parts/grip/");
			GUNPARTs::Set_Pre(&this->uperhandguard_data, "data/Guns/parts/uper_handguard/");
			GUNPARTs::Set_Pre(&this->underhandguard_data, "data/Guns/parts/under_handguard/");
			GUNPARTs::Set_Pre(&this->mount_base_data, "data/Guns/parts/mount_base/");
			GUNPARTs::Set_Pre(&this->mount_data, "data/Guns/parts/mount/");
			GUNPARTs::Set_Pre(&this->dustcover_data, "data/Guns/parts/dustcover/");
			GUNPARTs::Set_Pre(&this->foregrip_data, "data/Guns/parts/foregrip/");
			GUNPARTs::Set_Pre(&this->mazzule_data, "data/Guns/parts/mazzule/", EnumGunParts::PARTS_MAZZULE);
			GUNPARTs::Set_Pre(&this->stock_data, "data/Guns/parts/stock/", EnumGunParts::PARTS_STOCK);
			GUNPARTs::Set_Pre(&this->sight_data, "data/Guns/parts/sight/", EnumGunParts::PARTS_SIGHT);
			GUNPARTs::Set_Pre(&this->lam_data, "data/Guns/parts/lam/", EnumGunParts::PARTS_LAM);
			GUNPARTs::Set_Pre(&this->gun_data, "data/Guns/gun/", EnumGunParts::PARTS_BASE);					//GUNデータ
			GUNPARTs::Set_Pre(&this->magazine_data, "data/Guns/mag/", EnumGunParts::PARTS_MAGAZINE);		//MAGデータ
		}

		void Set() {
			for (auto& g : this->mazzule_data) { g.Set(&g - &this->mazzule_data.front()); }
			for (auto& g : this->grip_data) { g.Set(&g - &this->grip_data.front()); }
			for (auto& g : this->uperhandguard_data) { g.Set(&g - &this->uperhandguard_data.front()); }
			for (auto& g : this->underhandguard_data) { g.Set(&g - &this->underhandguard_data.front()); }
			for (auto& g : this->mount_base_data) { g.Set(&g - &this->mount_base_data.front()); }
			for (auto& g : this->mount_data) { g.Set(&g - &this->mount_data.front()); }
			for (auto& g : this->stock_data) { g.Set(&g - &this->stock_data.front()); }
			for (auto& g : this->dustcover_data) { g.Set(&g - &this->dustcover_data.front()); }
			for (auto& g : this->lam_data) { g.Set(&g - &this->lam_data.front()); }
			for (auto& g : this->foregrip_data) { g.Set(&g - &this->foregrip_data.front()); }
			for (auto& g : this->sight_data) { g.Set(&g - &this->sight_data.front()); }
			for (auto& g : this->gun_data) { g.Set(&g - &this->gun_data.front()); }							//GUNデータ2
			for (auto& g : this->magazine_data) { g.Set(&g - &this->magazine_data.front()); }				//MAGデータ2
		}
	};
};