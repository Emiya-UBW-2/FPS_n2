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

		void set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);

		}

		void Duplicate(Audios& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->shot = tgt.shot.Duplicate();
			this->slide = tgt.slide.Duplicate();
			this->trigger = tgt.trigger.Duplicate();
			this->mag_down = tgt.mag_down.Duplicate();
			this->mag_set = tgt.mag_set.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}

		void Dispose() {
			this->shot.Dispose();
			this->slide.Dispose();
			this->trigger.Dispose();
			this->mag_down.Dispose();
			this->mag_set.Dispose();
		}
	};
	//銃、マガジン共通モデル
	class Models {
	private:
	public:
		GraphHandle UIScreen;
		MV1 model;
		std::string name;
		void set(std::string path, std::string named) {
			name = named;
			MV1::Load(path + named + "/model.mv1", &model, true);
			UIScreen = GraphHandle::Load(path + named + "/pic.bmp");
		}
	};
public:
	class Chara;
	//必要アイテム
	class need_attach {
	private:
		size_t type = 0;
		bool ok = false;
	public:
	};
	//装着可能アイテム
	class can_attach {
	private:
		std::string name;
	public:
	};
	//パーツオブジェ
	class Parts_info {
	private:
		size_t type = 0;
		std::vector<Parts_info> parts;
		std::vector<can_attach> cans;
		std::vector<need_attach> needs;
	public:

	};
	//弾オブジェ
	class Ammos_info {
	private:
		size_t type = 0;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ
	public:

	};
	//マガジンオブジェ
	class Magazines_info {
	private:
		size_t type = 0;
		size_t type_gun = 0;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ
		size_t capa = 1;
		std::vector<Ammos_info> ammo;
	public:
	};
	//銃オブジェ
	class Gun_info {
	private:
		size_t type_gun = 0;
		std::vector<Parts_info> parts;
		std::vector<can_attach> cans;
		std::vector<need_attach> needs;
	public:
	};
	//アイテム
	class Items_info {
	private:
		int cate = 0;		//種類
		Parts_info parts;
		Ammos_info ammos;
		Magazines_info magazines;
		Gun_info guns;
	public:
	};
public:
	//弾データ
	class Ammo_info {
	private:
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ

		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_float(mdata);//ダメージ
			FileRead_close(mdata);
		}
	};
	//実際に発射される弾
	class Ammos {
	private:
	public:
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammo_info* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos;
		VECTOR_ref vec;
		void ready() {
			this->flug = false;
		}
		void set(Ammo_info* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
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
		void update(Chara* c, std::vector<Chara>* chara, std::unique_ptr<Y, D>& mapparts) {
			if (this->flug) {
				this->repos = this->pos;
				this->pos += this->vec * (this->spec->speed / GetFPS());
				//判定
				{
					auto p = mapparts->map_col_line(this->repos, this->pos, 0);
					if (p.HitFlag == TRUE) {
						this->pos = p.HitPosition;
					}
					//*
					for (auto& tgt : *chara) {
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,0);
							if (q.HitFlag == TRUE) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;

								tgt.HP = std::clamp(tgt.HP - 20, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 50;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,1);
							if (q.HitFlag == TRUE) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - 100, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 70;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,2);
							if (q.HitFlag == TRUE) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;

								tgt.HP = std::clamp(tgt.HP - 34, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 100;
								}
								break;
							}
						}
					}
					if (p.HitFlag == TRUE && this->flug) {
						this->flug = false;
						c->effcs_gndhit[c->use_effcsgndhit].set(this->pos, p.Normal, 0.025f / 0.1f);
						++c->use_effcsgndhit %= c->effcs_gndhit.size();
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
		void draw() {
			if (this->flug) {
				DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->caliber - 0.00762f) * 0.1f + 0.00762f), this->color, GetColor(255, 255, 255));
			}
		}
	};
	/**/
	//マガジンデータ
	class Mags {
	private:
	public:
		size_t id = 0;
		Models mod;
		/**/
		size_t cap = 1;
		std::vector<Ammo_info> ammo;
		/**/
		void set_data() {
			this->mod.model.SetMatrix(MGetIdent());
			{
				int mdata = FileRead_open(("data/mag/" + this->mod.name + "/data.txt").c_str(), FALSE);
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
		}
	};
	/**/
	//銃データ
	class Guns{
	private:
	public:
		size_t id = 0;
		Models mod;
		/**/
		std::vector<frames> frame;
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
		Mags* magazine;
		/**/
		void set_data(std::vector<Mags>& mag_data) {
			this->mod.model.SetMatrix(MGetIdent());
			{
				//フレーム
				this->frame.resize(6);
				this->frame[0].first = INT_MAX;
				this->frame[1].first = INT_MAX;
				this->frame[2].first = INT_MAX;
				this->frame[3].first = INT_MAX;
				this->frame[4].first = INT_MAX;
				this->frame[5].first = INT_MAX;
				for (int i = 0; i < this->mod.model.frame_num(); i++) {
					std::string s = this->mod.model.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//マガジン
						this->frame[0].second = this->mod.model.frame(i);
						this->frame[1].first = i + 1;
						this->frame[1].second = this->mod.model.frame(i + 1);
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[2].first = i;//マズル
					}
					else if (s.find("LEFT") != std::string::npos) {
						this->frame[3].first = i;//左手
					}
					else if (s.find("site") != std::string::npos) {
						this->frame[4].first = i;//アイアンサイト
						this->frame[4].second = this->mod.model.frame(i);
					}
					else if (s.find("RIGHT") != std::string::npos) {
						this->frame[5].first = i;//左手
					}
				}
				//テキスト
				{
					int mdata = FileRead_open(("data/gun/" + this->mod.name + "/data.txt").c_str(), FALSE);
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
					auto tmp = getparams::_str(mdata);
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
					for (auto& g : mag_data) {
						if (tmp == g.mod.name) {
							this->magazine = &g;
						}
					}
				}
			}
		}
	};
	/**/
public:
	//player
	class Chara {
	private:
		struct ammo_state {//弾データ
			size_t ammo_in = 0;
		};
		struct mag_state {//マガジンデータ
			size_t ammo_in = 0;
		};
		struct gun_state {//銃のデータ
			size_t ammo_cnt = 0;		//装弾数カウント
			size_t ammo_total = 0;		//所持弾数
			std::vector<size_t> mag_in;	//マガジン内
			uint8_t select = 0;			//セレクター

			void init() {
				this->ammo_total = 0;
				this->select = 0;
			}
			//マガジンを1つ追加(装填無し)
			void mag_plus(Mags* magazine) {
				this->ammo_total += magazine->cap;
				this->mag_in.insert(this->mag_in.end(), magazine->cap);
			}
			//マガジンを1つ追加(装填あり)
			void mag_insert(Mags* magazine) {
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
			//射撃する
			void mag_shot(const bool& reloadf) {
				this->ammo_cnt--;
				this->ammo_total--;
				if (!reloadf && this->mag_in.size() >= 1 && this->mag_in.front() > 0) {
					this->mag_in.front()--;
				}
			}
			//スライドを引いて1発装填
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
		std::array<Ammos, 64> bullet;
		size_t use_bullet = 0;
		//所持弾数などのデータ
		std::vector<gun_state> gun_stat;
		/*武器ポインタ*/
		Guns* gun_ptr = nullptr;				//現在使用中の武装
		/*モデル、音声*/
		Audios audio;
		MV1 obj_gun, obj_mag;
		MV1 body, col;
		/**/
		float reload_cnt = 0.f;//リロード開始までのカウント
		switchs trigger;//トリガー
		bool gunf = false;//射撃フラグ
		switchs selkey;//トリガー
		bool LEFT_hand = false;//左手を添えているか
		bool reloadf = false;
		bool down_mag = false;
		//プレイヤー座標系
		VECTOR_ref pos;							//位置
		MATRIX_ref mat;							//位置
		VECTOR_ref spawn_pos;					//spawn
		MATRIX_ref spawn_mat;					//spawn
		VECTOR_ref add_pos, add_pos_buf;		//移動

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
		VECTOR_ref pos_HMD, rec_HMD;
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
		float HP_r = 100;
		int HP = 100;
		int HP_full = 100;

		bool kill_f = false;
		size_t kill_id = 0;
		float kill_time = 0.f;
		int kill_streak = 0;
		bool hit_f{ false };
		float hit_time = 0.f;

		int score = 0;
		//
		void set(std::vector<Guns>& gun_data, const size_t& itr, MV1& body_, MV1& col_) {
			this->gun_ptr = &gun_data[itr];
			//身体
			body_.DuplicateonAnime(&this->body);
			frame_.get_frame(this->body, &this->head_hight);
			col_.DuplicateonAnime(&this->col);
			colframe_.get_frame(this->col,&this->head_hight2);
			for (int i = 0; i < col.mesh_num(); i++) {
				col.SetupCollInfo(8, 8, 8, -1, i);
			}

			this->gun_ptr->mod.model.DuplicateonAnime(&this->obj_gun);
			this->obj_mag = this->gun_ptr->magazine->mod.model.Duplicate();
			this->LEFT_hand = false;

			//マガジン
			this->gun_stat.resize(gun_data.size());
			for (auto& s : this->gun_stat) {
				s.init();
			}

			this->gunf = false;
			this->vecadd_RIGHTHAND = VGet(0, 0, 1.f);
			this->vecadd_RIGHTHAND_p = this->vecadd_RIGHTHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->selkey.ready(false);
			for (auto& a : this->obj_gun.get_anime()) {
				a.per = 0.f;
			}
			for (auto& a : this->bullet) {
				a.ready();
			}
			this->audio.Duplicate(this->gun_ptr->audio);
			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;
			score = 0;
		}
		void spawn(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) {
			this->spawn_pos = pos_;
			this->pos = this->spawn_pos;
			this->spawn_mat = mat_H;
			this->mat = this->spawn_mat;

			this->add_ypos = 0.f;
			this->body.SetMatrix(MATRIX_ref::Mtrans(this->pos - this->rec_HMD));
			this->body.PhysicsResetState();

			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;

			this->gun_stat[this->gun_ptr->id].init();
			this->gun_stat[this->gun_ptr->id].mag_insert(this->gun_ptr->magazine);			//マガジン+1
			this->gun_stat[this->gun_ptr->id].mag_insert(this->gun_ptr->magazine);			//マガジン+1
			this->gun_stat[this->gun_ptr->id].mag_insert(this->gun_ptr->magazine);			//マガジン+1

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度
		}

		void Draw_chara() {
			this->body.DrawModel();
			this->obj_gun.DrawModel();
			//this->col.DrawModel();

			if ((!this->reloadf || this->down_mag) && this->gun_stat[this->gun_ptr->id].mag_in.size() >= 1) {
				this->obj_mag.DrawModel();
			}
		}
		void Draw_ammo() {
			for (auto& a : this->bullet) {
				a.draw();
			}
		}
		void Delete_chara() {
			this->gun_ptr = nullptr;
			this->body.Dispose();
			this->col.Dispose();
			this->obj_gun.Dispose();
			this->obj_mag.Dispose();
			this->audio.Dispose();
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
			for (auto& t : this->effcs_gndhit) {
				t.handle.Dispose();
			}
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
		Guns* ptr = nullptr;
		//マガジン専用パラメーター
		Mags magazine;
		//
		void Set_item(Guns*gundata, const VECTOR_ref& pos_, const MATRIX_ref& mat_, int cat) {
			bool choose = true;
			this->cate = cat;
			switch (this->cate) {
			case 1:
				choose = gundata->magazine->mod.name.find("none") == std::string::npos;
				break;
			default:
				choose = true;
				break;
			}

			if (choose) {
				this->pos = pos_;
				this->add = VGet(0, 0, 0);
				this->mat = mat_;
				//
				this->ptr = gundata;
				switch (this->cate) {
				case 1:
					this->obj = this->ptr->magazine->mod.model.Duplicate();
					break;
				default:
					this->obj = this->ptr->mod.model.Duplicate();
					break;
				}
			}
			else {
				this->Delete_item();
			}
		}
		template<class Y, class D>
		void Get_item_1(std::vector<Items>& item, std::unique_ptr<Y, D>& mapparts) {
			if (this->ptr != nullptr) {
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add;
				this->add.yadd(M_GR / powf(GetFPS(), 2.f));
				for (auto& p : item) {
					if (p.ptr != nullptr && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.1f) {
							p.add.xadd((p.pos - this->pos).x()*10.f / GetFPS());
							p.add.zadd((p.pos - this->pos).z()*10.f / GetFPS());
							this->add.xadd((this->pos - p.pos).x()*10.f / GetFPS());
							this->add.zadd((this->pos - p.pos).z()*10.f / GetFPS());
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
			}
		}

		template<class Y, class D>
		void Get_item_2( Chara& chara, std::unique_ptr<Y, D>& mapparts) {
			if (this->ptr != nullptr) {
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
						chara.canget_mag = this->ptr->magazine->mod.name;
						if (chara.getmag.second == 1 && this->magazine.cap != 0) {
							chara.gun_stat[this->ptr->id].mag_plus(&(this->magazine));
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
					if (chara.canget_magitem && chara.canget_mag == this->ptr->mod.name) {
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