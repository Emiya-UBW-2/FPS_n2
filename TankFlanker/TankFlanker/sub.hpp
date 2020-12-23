#pragma once

#define NOMINMAX
#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include<fstream>
#include "DXLib_ref/DXLib_ref.h"

#define FRAME_RATE 90.f
//
enum Effect {
	ef_fire, //発砲炎
	ef_reco, //小口径跳弾
	ef_smoke, //銃の軌跡
	ef_gndsmoke,//地面の軌跡
	effects, //読み込む
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
		SoundHandle mag_set;
		SoundHandle case_down;

		void set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_" + getparams::_str(mdata) + ".wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);
		}
	};
	//弾データ
	class Ammos {
	private:
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ

		MV1 ammo;
		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_float(mdata);//ダメージ
			FileRead_close(mdata);

			MV1::Load("data/ammo/" + this->name + "/ammo.mv1", &ammo, true);
		}
	};
	//マガジンデータ
	class Mags {
	private:
	public:
		std::string name;
		int cap = 1;
		std::vector<Ammos> ammo;

		MV1 mag;
		void set(void) {
			if (this->name.find("none") == std::string::npos) {
				int mdata = FileRead_open(("data/mag/" + this->name + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_int(mdata);//口径
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
				MV1::Load("data/mag/" + this->name + "/mag.mv1", &mag, true);
			}
		}
	};
	//実際に発射される弾
	class ammos {
	private:
	public:
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammos* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;

		void ready() {
			this->flug = false;
			this->color = GetColor(255, 255, 172);
		}
		void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->spec = spec_t;
			this->pos = pos_t;
			this->vec = vec_t;
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->yadd = 0.f;
			this->repos = this->pos;
		}
		void draw() {
			if (this->flug) {
				DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->caliber - 0.00762f) * 0.1f + 0.00762f), this->color, GetColor(255, 255, 255));
			}
		}
	};
public:
	//銃データ
	class Gun {
	private:
		class Models {
		private:
		public:
			GraphHandle UIScreen;
			MV1 obj;
			std::string name;
			void set(std::string named) {
				name = named;
				MV1::Load("data/gun/" + named + "/model.mv1", &obj, true);
				UIScreen = GraphHandle::Load("data/gun/" + named + "/pic.bmp");
			}
		};
	public:
		int id = 0;
		std::string name;
		std::vector<frames> frame;
		std::vector <uint8_t> select;//セレクター
		Models mod;
		Audios audio;
		size_t ammo_max = 0;//マガジンの装弾数
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//再装填時間
		float ammo_speed = 100.f;//弾速
		float ammo_pene = 10.f;//貫通
		float ammo_damege = 10.f;//ダメージ
		std::vector<Ammos> ammo;
		Mags mag;
		void set_data() {
			this->name = this->mod.name;
			this->mod.obj.SetMatrix(MGetIdent());
			{
				//フレーム
				this->frame.resize(6);
				this->frame[0].first = INT_MAX;
				this->frame[1].first = INT_MAX;
				this->frame[2].first = INT_MAX;
				this->frame[3].first = INT_MAX;
				this->frame[4].first = INT_MAX;
				this->frame[5].first = INT_MAX;
				for (int i = 0; i < this->mod.obj.frame_num(); i++) {
					std::string s = this->mod.obj.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//マガジン
						this->frame[0].second = this->mod.obj.frame(i);
						this->frame[1].first = i + 1;
						this->frame[1].second = this->mod.obj.frame(i + 1);
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[2].first = i;//マズル
					}
					else if (s.find("LEFT") != std::string::npos) {
						this->frame[3].first = i;//左手
					}
					else if (s.find("site") != std::string::npos) {
						this->frame[4].first = i;//アイアンサイト
						this->frame[4].second = this->mod.obj.frame(i);
					}
					else if (s.find("RIGHT") != std::string::npos) {
						this->frame[5].first = i;//左手
					}
				}
				//テキスト
				{
					int mdata = FileRead_open(("data/gun/" + this->name + "/data.txt").c_str(), FALSE);
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
					this->audio.set(mdata);
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
					this->mag.name = getparams::_str(mdata);
					//反動
					this->recoil_xup = getparams::_float(mdata);
					this->recoil_xdn = getparams::_float(mdata);
					this->recoil_yup = getparams::_float(mdata);
					this->recoil_ydn = getparams::_float(mdata);
					//リロードタイム
					this->reload_time = getparams::_float(mdata);
					FileRead_close(mdata);
					//
					for (auto& a : this->ammo) {
						a.set();
					}
					this->mag.set();

					this->ammo_max = this->mag.cap;
				}
			}
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
			void ready() {
				this->cnt = -1.f;
				this->pos = VGet(0, 0, 0);
				this->mat = MGetIdent();
			}
			void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t, const MATRIX_ref& mat_t) {
				this->second.Dispose();
				this->second = spec_t->ammo.Duplicate();
				this->cnt = 0.f;
				this->pos = pos_t;//排莢
				this->add = vec_t;//排莢ベクトル
				this->mat = mat_t;
			}
			template<class Y, class D>
			void get(std::unique_ptr<Y, D>& mapparts,Chara&c) {
				if (this->cnt >= 0.f) {
					float fps = GetFPS();

					this->cnt += 1.f / fps;
					this->pos += this->add*(float(200 + GetRand(1000)) / 1000.f);
					this->add.yadd(-9.8f / powf(fps, 2.f));

					auto pp = mapparts->map_col_line(this->pos + VGet(0, 1.f, 0), this->pos - VGet(0, 0.008f, 0), 0);
					if (pp.HitFlag == 1) {
						this->pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.008f, 0);
						this->add += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(this->add*-1.f)*1.25f);
						easing_set(&this->add, VGet(0, 0, 0), 0.95f);
						if (!this->down) {
							c.audio.case_down.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
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
			void draw() {
				if (this->cnt >= 0.f) {
					this->second.DrawModel();
				}
			}
			void delete_cart() {
				this->second.Dispose();
			}
		};
		struct gun_state {
			int in = 0;				//所持弾数
			std::vector<int> mag_in;	//マガジン内
			uint8_t select = 0;			//セレクター
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

			void get_frame(MV1& obj_,float*head_hight) {
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
					else if (p.find("右手首先") != std::string::npos) {
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
					else if (p.find("左手首先") != std::string::npos) {
						this->LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
				}
			}

			void copy_frame(MV1& obj_mine, fs& frame_tgt_,MV1* obj_tgt) {
				obj_tgt->SetMatrix(obj_mine.GetMatrix());
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.head_f.first, obj_mine.GetFrameLocalMatrix(this->head_f.first));
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTeye_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTeye_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTeye_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTeye_f.first));
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.first, obj_mine.GetFrameLocalMatrix(this->bodyg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.first, obj_mine.GetFrameLocalMatrix(this->bodyc_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.first, obj_mine.GetFrameLocalMatrix(this->bodyb_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.body_f.first, obj_mine.GetFrameLocalMatrix(this->body_f.first));
				//右手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHThand2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHThand_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTarm2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTarm1_f.first));
				//左手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFThand2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.first, obj_mine.GetFrameLocalMatrix(this->LEFThand_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTarm2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTarm1_f.first));
				//右手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTreg2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTreg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTfoot2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTfoot1_f.first));
				//左手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTreg2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTreg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTfoot2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTfoot1_f.first));

				for (int i = 0; i < 9; i++) {
					obj_tgt->get_anime(i).per = obj_mine.get_anime(i).per;
					obj_tgt->get_anime(i).time = obj_mine.get_anime(i).time;
				}
			}
		};
	public:
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		size_t use_effcsgun = 0;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<ammos, 64> bullet;
		size_t use_bullet = 0;
		//所持弾数などのデータ
		std::vector<gun_state> gun_stat;
		/*武器ポインタ*/
		Gun* ptr_now = nullptr;				//現在使用中の武装
		size_t ammo_cnt = 0;				//装弾数カウント
		Gun* gun_ptr;						//銃スロット
		/*モデル、音声*/
		Audios audio;
		MV1 obj, mag, body;
		MV1 col;
		/**/
		float reload_cnt = 0.f;//リロード開始までのカウント
		switchs trigger;//トリガー
		bool gunf = false;//射撃フラグ
		switchs selkey;//トリガー
		bool LEFT_hand = false;//左手を添えているか
		bool reloadf = false;
		bool down_mag = false;
		//プレイヤー座標系
		VECTOR_ref spawn_pos;							//移動
		VECTOR_ref pos, add_pos, add_pos_buf;							//移動
		switchs ads, squat;
		bool wkey = false;
		bool skey = false;
		bool akey = false;
		bool dkey = false;
		bool running = false;										//走るか否か
		VECTOR_ref gunpos;											//マウスエイム用銃座標
		switchs getmag;
		//マガジン座標系
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//頭部座標系
		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;

		VECTOR_ref rec_HMD;

		float add_ypos = 0.f;//垂直加速度
		float body_xrad = 0.f;//胴体角度
		float body_yrad = 0.f;//胴体角度
		float body_zrad = 0.f;//胴体角度
		//フレーム
		fs frame_;
		float head_hight = 0.f;
		float head_hight2 = 0.f;
		//右手座標系
		VECTOR_ref pos_RIGHTHAND;
		MATRIX_ref mat_RIGHTHAND;
		VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
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
		bool canget_magitem = false;
		std::string canget_mag;
		bool start_c = true;
		//
		void Ready_chara(std::vector<Gun>& gun_data, const size_t& itr, MV1& body_, MV1& col_) {
			this->gun_ptr = &gun_data[itr];
			this->gun_stat.resize(gun_data.size());
			for (auto& s : this->gun_stat) {
				s.in = 0;
				s.select = 0;
			}
			//身体
			body_.DuplicateonAnime(&this->body);
			frame_.get_frame(this->body, &this->head_hight);
			col_.DuplicateonAnime(&this->col);
			colframe_.get_frame(this->col,&this->head_hight2);
			col.SetupCollInfo(8, 8, 8,-1);
		}
		void Set_chara_Position(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) {
			this->spawn_pos = pos_;
			this->pos = this->spawn_pos;
			this->mat_HMD = mat_H;
		}
		void Set_chara() {
			this->ptr_now = this->gun_ptr;
			this->ptr_now->mod.obj.DuplicateonAnime(&this->obj);
			this->mag = this->ptr_now->mag.mag.Duplicate();
			this->LEFT_hand = false;
			
			//マガジン+1
			this->gun_stat[this->ptr_now->id].in += this->ptr_now->mag.cap;
			this->gun_stat[this->ptr_now->id].mag_in.insert(this->gun_stat[this->ptr_now->id].mag_in.end(), this->ptr_now->mag.cap);
			//
			if (this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
				this->ammo_cnt = this->gun_stat[this->ptr_now->id].mag_in.front();//改良
			}
			else {
				this->ammo_cnt = std::clamp<size_t>(this->gun_stat[this->ptr_now->id].in, 0, this->ptr_now->ammo_max);//改良
			}


			this->gunf = false;
			this->vecadd_RIGHTHAND = VGet(0, 0, 1.f);
			this->vecadd_RIGHTHAND_p = this->vecadd_RIGHTHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->selkey.second = 0;
			for (auto& a : this->obj.get_anime()) {
				a.per = 0.f;
			}
			for (auto& a : this->bullet) {
				a.ready();
			}
			SetCreate3DSoundFlag(TRUE);
			this->audio.shot = this->ptr_now->audio.shot.Duplicate();
			this->audio.slide = this->ptr_now->audio.slide.Duplicate();
			this->audio.trigger = this->ptr_now->audio.trigger.Duplicate();
			this->audio.mag_down = this->ptr_now->audio.mag_down.Duplicate();
			this->audio.mag_set = this->ptr_now->audio.mag_set.Duplicate();
			this->audio.case_down = this->ptr_now->audio.case_down.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void Draw_chara() {
			this->body.DrawModel();
			this->obj.DrawModel();
			DrawLine3D(this->obj.frame(this->ptr_now->frame[2].first).get(), (this->obj.frame(this->ptr_now->frame[2].first) - this->obj.GetMatrix().zvec()*100.f).get(), GetColor(255, 0, 0));
			//this->col.DrawModel();

			if ((!this->reloadf || this->down_mag) && this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
				this->mag.DrawModel();
			}
		}
		void Draw_ammo() {
			for (auto& a : this->bullet) {
				a.draw();
			}
		}
		void Delete_chara() {
			this->ptr_now = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
			this->audio.shot.Dispose();
			this->audio.slide.Dispose();
			this->audio.trigger.Dispose();
			this->audio.mag_down.Dispose();
			this->audio.mag_set.Dispose();
			this->audio.case_down.Dispose();
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
			for (auto& t : this->effcs_gndhit) {
				t.handle.Dispose();
			}
		}
		void Init_chara() {
			this->body.Dispose();
			this->col.Dispose();
			Delete_chara();
			gun_ptr = nullptr;
		}
	};
	//アイテム
	class Items {
	private:
	public:
		//種類
		int cate = 0;
		//共通
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		Gun* ptr = nullptr;
		//マガジン専用パラメーター
		int cap = 0;
		//
		void Set_item(Gun*gundata, const VECTOR_ref& pos_, const MATRIX_ref& mat_, int cat) {
			bool choose = true;
			this->cate = cat;
			switch (this->cate) {
			case 1:
				choose = gundata->mag.name.find("none") == std::string::npos;
				break;
			default:
				choose = true;
				break;
			}

			if (choose) {
				this->pos = pos_;
				this->add = VGet(0, 0, 0);
				this->mat = mat_;
				//手
				this->ptr = gundata;
				switch (this->cate) {
				case 1:
					this->obj = this->ptr->mag.mag.Duplicate();
					break;
				default:
					this->obj = this->ptr->mod.obj.Duplicate();
					break;
				}
			}
			else {
				this->Delete_item();
			}
		}
		template<class Y, class D>
		void Get_item( Chara& chara, std::vector<Items>& item, std::unique_ptr<Y, D>& mapparts) {
			if (this->ptr != nullptr) {
				float fps = GetFPS();
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add;
				this->add.yadd(-0.98f / powf(fps, 2.f));
				for (auto& p : item) {
					if (p.ptr != nullptr && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.1f) {
							p.add.xadd((p.pos - this->pos).x()*10.f / fps);
							p.add.zadd((p.pos - this->pos).z()*10.f / fps);
							this->add.xadd((this->pos - p.pos).x()*10.f / fps);
							this->add.zadd((this->pos - p.pos).z()*10.f / fps);
						}
					}
				}
				auto pp = mapparts->map_col_line(this->pos + VGet(0, 1.f, 0), this->pos - VGet(0, 0.05f, 0), 0);
				if (pp.HitFlag == 1) {
					this->pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
					this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(pp.Normal));
					easing_set(&this->add, VGet(0, 0, 0), 0.8f);
				}
				//
				VECTOR_ref startpos = chara.pos_LEFTHAND;
				VECTOR_ref endpos = startpos - chara.mat_LEFTHAND.zvec()*2.6f;
				auto p = mapparts->map_col_line(startpos, endpos, 0);
				if (p.HitFlag == 1) {
					endpos = p.HitPosition;
				}
				bool zz=false;
				switch (this->cate){
				case 1:
					zz = (Segment_Point_MinLength(startpos.get(), endpos.get(), this->pos.get()) <= 0.2f);
					chara.canget_magitem |= zz;
					if (zz) {
						chara.canget_mag = this->ptr->mag.name;
						if (chara.getmag.second == 1) {
							chara.gun_stat[this->ptr->id].in += this->cap;
							chara.gun_stat[this->ptr->id].mag_in.insert(chara.gun_stat[this->ptr->id].mag_in.end(), this->cap);
							if (chara.gun_stat[this->ptr->id].mag_in.size() == 1) {
								chara.reloadf = true;
							}
							this->Delete_item();
						}
					}
					break;
				default:
					break;
				}
			}
			//個別
		}
		void Draw_item() {
			if (this->ptr != nullptr) {
				this->obj.DrawModel();
			}
		}
		void Draw_item(Chara& chara) {
			if (this->ptr != nullptr) {
				if (this->cate == 1) {
					if (chara.canget_magitem && chara.canget_mag == this->ptr->name) {
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
		}
		void Delete_item() {
			this->ptr = nullptr;
			this->obj.Dispose();
		}
	};
};
//