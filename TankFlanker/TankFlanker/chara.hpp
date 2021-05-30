#pragma once

#include "sub.hpp"


class PLAYERclass : Mainclass {
public:
	class Chara {
		//そのまま
		//引き継ぐ
		std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs = nullptr;
		std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts = nullptr;
		//仮
		std::unique_ptr<DeBuG, std::default_delete<DeBuG>>* DebugPTs = nullptr;
	private:
		//実際に発射される弾
		class BULLETS {
		private:
			//引き継ぐ
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs = nullptr;

			bool hit_Flag = false;
			float hit_cnt = 0.f;
			bool Flag = false;
			bool DrawFlag = false;
			float cnt = 0.f;
			Ammos* spec = nullptr;
			float yadd = 0.f;
			VECTOR_ref pos;
			MATRIX_ref mat;
			VECTOR_ref repos;
		public:
			float hit_alpha = 0.f;
			int hit_window_x = 0;
			int hit_window_y = 0;
		public:
			void Set(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t) noexcept {
				MAPPTs = MAPPTs_t;
				this->Flag = false;
				this->DrawFlag = false;
			}
			void Put(Ammos* spec_t, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
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
			void UpDate(Chara* c, std::vector<Chara>* chara, HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p) noexcept {
				if (this->Flag) {
					this->repos = this->pos;
					this->pos += this->mat.zvec() * (this->spec->get_speed() / FPS);
					//判定
					{
						auto p = (*MAPPTs)->map_col_line(this->repos, this->pos);
						if (p.HitFlag) {
							this->pos = p.HitPosition;
						}
						//*
						for (auto& tgt : *chara) {
							if (&tgt == c || !tgt.get_alive()) {
								continue;
							}
							if (tgt.set_ref_col(this->repos, this->pos)) {
								//HEAD
								{
									auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 0);
									if (q.HitFlag) {
										this->pos = q.HitPosition;
										//hit
										//c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
										c->effcs[ef_hitblood].set(this->pos, q.Normal, 0.1f / 0.1f);
										//
										this->hit_Flag = true;
										this->Flag = false;

										auto old = tgt.HP;
										tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 3, 0, tgt.HP_full);
										tgt.HP_parts[0] = std::clamp(tgt.HP_parts[0] - this->spec->get_damage() * 3, 0, tgt.HP_full);
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
										{
											float x_1 = sinf(tgt.body_yrad);
											float y_1 = cosf(tgt.body_yrad);
											auto vc = (c->get_pos() - tgt.get_pos()).Norm();
											tgt.got_damage_.resize(tgt.got_damage_.size() + 1);
											tgt.got_damage_.back().alpfa = 1.f;
											tgt.got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
										}
										if (!tgt.get_alive()) {
											c->scores.set_kill(&tgt - &(*chara)[0], 70);
											tgt.scores.set_death(&(*c) - &(*chara)[0]);
											tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
										}
										else {
											tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
										}
										break;
									}
								}
								//BODY
								{
									auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 1);
									if (q.HitFlag) {
										this->pos = q.HitPosition;
										//hit
										//c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
										c->effcs[ef_hitblood].set(this->pos, q.Normal, 0.1f / 0.1f);

										//
										this->hit_Flag = true;
										this->Flag = false;

										auto old = tgt.HP;
										tgt.HP = std::clamp(tgt.HP - this->spec->get_damage(), 0, tgt.HP_full);
										tgt.HP_parts[1] = std::clamp(tgt.HP_parts[1] - this->spec->get_damage(), 0, tgt.HP_full);
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
										{
											float x_1 = sinf(tgt.body_yrad);
											float y_1 = cosf(tgt.body_yrad);
											auto vc = (c->get_pos() - tgt.get_pos()).Norm();
											tgt.got_damage_.resize(tgt.got_damage_.size() + 1);
											tgt.got_damage_.back().alpfa = 1.f;
											tgt.got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
										}
										if (!tgt.get_alive()) {
											c->scores.set_kill(&tgt - &(*chara)[0], 100);
											tgt.scores.set_death(&(*c) - &(*chara)[0]);
											tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
										}
										else {
											tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
										}
										break;
									}
								}
								//LEFTHAND
								{
									auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 2);
									if (q.HitFlag) {
										this->pos = q.HitPosition;
										//hit
										//c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
										c->effcs[ef_hitblood].set(this->pos, q.Normal, 0.1f / 0.1f);

										//
										this->hit_Flag = true;
										this->Flag = false;

										auto old = tgt.HP;
										tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 2 / 5, 0, tgt.HP_full);
										tgt.HP_parts[2] = std::clamp(tgt.HP_parts[2] - this->spec->get_damage() * 2 / 5, 0, tgt.HP_full);
										tgt.got_damage = old - tgt.HP;
										tgt.got_damage_color = GetColor(255, 255, 255);

										if (float(tgt.HP) / float(tgt.HP_full) <= 0.66) {
											tgt.got_damage_color = GetColor(255, 255, 0);
										}
										if (float(tgt.HP) / float(tgt.HP_full) <= 0.33) {
											tgt.got_damage_color = GetColor(255, 128, 0);
										}
										if ((this->spec->get_damage() * 2 / 5) != tgt.got_damage) {
											tgt.got_damage_color = GetColor(255, 0, 0);
										}

										tgt.got_damage_x = -255 + GetRand(255 * 2);
										tgt.got_damage_f = 1.f;
										{
											float x_1 = sinf(tgt.body_yrad);
											float y_1 = cosf(tgt.body_yrad);
											auto vc = (c->get_pos() - tgt.get_pos()).Norm();
											tgt.got_damage_.resize(tgt.got_damage_.size() + 1);
											tgt.got_damage_.back().alpfa = 1.f;
											tgt.got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
										}
										if (!tgt.get_alive()) {
											c->scores.set_kill(&tgt - &(*chara)[0], 100);
											tgt.scores.set_death(&(*c) - &(*chara)[0]);
											tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
										}
										else {
											tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
										}
										break;
									}
								}
								//RIGHTHAND
								{
									auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 3);
									if (q.HitFlag) {
										this->pos = q.HitPosition;
										//hit
										//c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
										c->effcs[ef_hitblood].set(this->pos, q.Normal, 0.1f / 0.1f);

										//
										this->hit_Flag = true;
										this->Flag = false;

										auto old = tgt.HP;
										tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 2 / 5, 0, tgt.HP_full);
										tgt.HP_parts[3] = std::clamp(tgt.HP_parts[3] - this->spec->get_damage() * 2 / 5, 0, tgt.HP_full);
										tgt.got_damage = old - tgt.HP;
										tgt.got_damage_color = GetColor(255, 255, 255);

										if (float(tgt.HP) / float(tgt.HP_full) <= 0.66) {
											tgt.got_damage_color = GetColor(255, 255, 0);
										}
										if (float(tgt.HP) / float(tgt.HP_full) <= 0.33) {
											tgt.got_damage_color = GetColor(255, 128, 0);
										}
										if ((this->spec->get_damage() * 2 / 5) != tgt.got_damage) {
											tgt.got_damage_color = GetColor(255, 0, 0);
										}

										tgt.got_damage_x = -255 + GetRand(255 * 2);
										tgt.got_damage_f = 1.f;
										{
											float x_1 = sinf(tgt.body_yrad);
											float y_1 = cosf(tgt.body_yrad);
											auto vc = (c->get_pos() - tgt.get_pos()).Norm();
											tgt.got_damage_.resize(tgt.got_damage_.size() + 1);
											tgt.got_damage_.back().alpfa = 1.f;
											tgt.got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
										}
										if (!tgt.get_alive()) {
											c->scores.set_kill(&tgt - &(*chara)[0], 100);
											tgt.scores.set_death(&(*c) - &(*chara)[0]);
											tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
										}
										else {
											tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
										}
										break;
									}
								}
								//LEG
								{
									auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1, 4);
									if (q.HitFlag) {
										this->pos = q.HitPosition;
										//hit
										//c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
										c->effcs[ef_hitblood].set(this->pos, q.Normal, 0.1f / 0.1f);

										//
										this->hit_Flag = true;
										this->Flag = false;

										auto old = tgt.HP;
										tgt.HP = std::clamp(tgt.HP - this->spec->get_damage() * 3 / 5, 0, tgt.HP_full);
										tgt.HP_parts[4] = std::clamp(tgt.HP_parts[4] - this->spec->get_damage() * 3 / 5, 0, tgt.HP_full);
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
										{
											float x_1 = sinf(tgt.body_yrad);
											float y_1 = cosf(tgt.body_yrad);
											auto vc = (c->get_pos() - tgt.get_pos()).Norm();
											tgt.got_damage_.resize(tgt.got_damage_.size() + 1);
											tgt.got_damage_.back().alpfa = 1.f;
											tgt.got_damage_.back().rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
										}
										if (!tgt.get_alive()) {
											c->scores.set_kill(&tgt - &(*chara)[0], 50);
											tgt.scores.set_death(&(*c) - &(*chara)[0]);
											tgt.audio.voice_death.play_3D(tgt.get_pos(), 10.f);
										}
										else {
											tgt.audio.voice_damage.play_3D(tgt.get_pos(), 10.f);
										}
										break;
									}
								}
							}
						}
						if (p.HitFlag) {
							if (this->Flag) {
								this->Flag = false;
								//弾痕
								c->effcs_gndhit[c->use_effcsgndhit].set(this->pos, p.Normal, 0.025f / 0.1f);
								++c->use_effcsgndhit %= c->effcs_gndhit.size();
								hit_obj_p.set(this->spec->get_caliber(), this->pos, p.Normal, this->mat.zvec());
								return;
							}
						}
						//血痕
						if (!this->Flag) {
							if (!p.HitFlag) {
								auto vec = (this->pos - this->repos).Norm();
								p = (*MAPPTs)->map_col_line(this->pos, this->pos + vec * 5.f);
							}
							if (p.HitFlag) {
								c->effcs_gndhit[c->use_effcsgndhit].set(p.HitPosition, p.Normal, 0.025f / 0.1f);
								++c->use_effcsgndhit %= c->effcs_gndhit.size();
								hit_b_obj_p.set(0.35f, p.HitPosition, p.Normal, this->mat.zvec());
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
					auto p = ConvWorldPosToScreenPos(this->pos.get());
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

			bool stay = false;
		public:
			auto& Get_Flag() const noexcept { return Flag; }
			auto& Get_pos() noexcept { return pos; }

			void Set(void) noexcept {
				this->Dispose();
			}
			void Put_first(MV1&model, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				this->Dispose();
				this->Flag = true;
				this->se_use = false;
				this->obj = model.Duplicate();
				this->pos = pos_t;//排莢
				this->mat = mat_t;
				this->stay = true;
			}

			void Put_second(const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				if (this->Flag) {
					if (this->stay) {
						this->pos = pos_t;//排莢
						this->mat = mat_t;
					}
				}
			}
			void Put_stay(const VECTOR_ref& vec_t, const float rate_t = 1) noexcept {
				if (this->Flag) {
					if (this->stay) {
						this->cnt = 1.5f;
						this->rate = rate_t;
						this->add = vec_t * this->rate;//排莢ベクトル
						this->stay = false;
					}
				}
			}

			void UpDate_pos() noexcept {
				if (this->Flag) {
					if (!this->stay) {
						this->cnt -= this->rate / FPS;
						this->pos += this->add*(float(200 + GetRand(1000)) / 1000.f);
						this->add.yadd(-9.8f / powf((FPS / this->rate), 2.f));
					}
				}
			}
			void UpDate_fall(const MV1_COLL_RESULT_POLY& mapcol, Chara& mine) noexcept {
				if (this->Flag) {
					if (this->stay) {
					}
					else {
						if (mapcol.HitFlag) {
							VECTOR_ref map_nomal = mapcol.Normal;
							this->pos = VECTOR_ref(mapcol.HitPosition) + VECTOR_ref::vget(0, 0.008f, 0);
							this->add += map_nomal * (map_nomal.dot(this->add*-1.f)*1.25f);
							easing_set(&this->add, VECTOR_ref::vget(0, 0, 0), 0.95f);
							if (!this->se_use) {
								mine.audio.case_down.play_3D(mine.pos_tt + mine.pos_gun, 1.f);
								this->se_use = true;
							}
							this->mat *= MATRIX_ref::RotVec2(this->mat.yvec(), map_nomal);
						}
						else {
							this->mat *= MATRIX_ref::RotY(deg2rad(360 * 3 + GetRand(360 * 3 * 2))* this->rate / FPS);
						}
					}
					this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
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
		public:
			GUNPARTs* ptr_mag = nullptr;
			MV1 obj_mag_;
			std::array<MV1, 2> obj_ammo;			//マガジン用弾
			std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム
			frames LEFT_mag_frame;
			std::array<frames, 2> magazine_f;		//マガジンフレーム
			size_t m_cnt = 0;
			size_t cap = 30;

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
			const auto LEFT_mag_pos(void) noexcept {
				return this->obj_mag_.frame(this->LEFT_mag_frame.first);
			}
		};
		class GUN_STATUS {
		private:
			bool in_chamber = false;			//チャンバー内
			size_t ammo_cnt = 0;				//チャンバー内
			uint8_t select = 0;					//セレクター
			std::vector<MAG_STATUS> magazine_in;	//マガジン内データ
			size_t get_magazine_in_not_full() noexcept {
				for (auto& m : this->magazine_in) {
					if ((&m != &this->magazine_in.front()) && (m.m_cnt != m.cap)) {
						return &m - &this->magazine_in.front();
					}
				}
				return 0;
			}
		public:
			uint8_t selecter = 0;					//セレクター
			//
			auto& get_magazine_in() noexcept { return magazine_in; }
			//マガジンを持っているか
			bool hav_mags(void) const noexcept { return magazine_in.size() > 0; }
			//チャンバーに弾があるか否か
			bool not_chamber_EMPTY(void) const noexcept { return in_chamber; }
			//マグ順ソート
			void sort_magazine() noexcept {
				std::sort(this->magazine_in.begin() + 1, this->magazine_in.end(), [](MAG_STATUS& a, MAG_STATUS& b) { return a.m_cnt > b.m_cnt; });
			}
			//弾抜き、装填
			void load_magazine() noexcept {
				size_t siz = this->get_magazine_in_not_full();
				auto be_ = std::clamp<size_t>(this->magazine_in.back().m_cnt, 0, magazine_in[siz].cap - this->magazine_in[siz].m_cnt);
				this->magazine_in.back().m_cnt -= be_;
				this->magazine_in[siz].m_cnt += be_;
			}
			//
			void Set(void) noexcept {
				this->magazine_in.clear();
				in_chamber = false;
			}
			//マガジンを1つ追加(装填無し)
			void magazine_plus(Items* magazine_item) noexcept {
				this->magazine_in.resize(this->magazine_in.size() + 1);
				this->magazine_in.back().m_cnt = magazine_item->get_magazine().mag_cnt;

				auto& m_in = this->magazine_in.back();
				m_in.ptr_mag = magazine_item->get_ptr_mag();
				m_in.cap = m_in.ptr_mag->mag_cnt;
				m_in.obj_mag_ = m_in.ptr_mag->mod.get_model().Duplicate();
				m_in.obj_ammo[0] = m_in.ptr_mag->ammo[0].get_model_full().Duplicate();
				m_in.obj_ammo[1] = m_in.ptr_mag->ammo[0].get_model_full().Duplicate();
				//m_in.LEFT_mag_frame;
				//m_in.magazine_ammo_f[0];
				//m_in.magazine_ammo_f[1];
			}
			void magazine_insert(GUNPARTs* magazine_parts) noexcept {
				this->magazine_in.resize(this->magazine_in.size() + 1);
				this->magazine_in.back().m_cnt = magazine_parts->mag_cnt;

				auto& m_in = this->magazine_in.back();
				m_in.ptr_mag = magazine_parts;
				m_in.cap = m_in.ptr_mag->mag_cnt;
				m_in.obj_mag_ = m_in.ptr_mag->mod.get_model().Duplicate();
				m_in.obj_ammo[0] = m_in.ptr_mag->ammo[0].get_model_full().Duplicate();
				m_in.obj_ammo[1] = m_in.ptr_mag->ammo[0].get_model_full().Duplicate();
				//m_in.LEFT_mag_frame;
				//m_in.magazine_ammo_f[0];
				//m_in.magazine_ammo_f[1];

				if (this->magazine_in.size() == 1) {
					this->magazine_in.back().m_cnt--;
					in_chamber = true;
				}
			}
			//射撃する
			void magazine_shot(const bool reloadf) noexcept {
				if (reloadf) {
					this->in_chamber = false;
				}
				else {
					if (this->hav_mags()) {
						if (this->magazine_in.front().m_cnt > 0) {
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
			//スライドを引いて装填
			void magazine_slide(void) noexcept {
				if (!this->not_chamber_EMPTY()) {
					this->magazine_in.front().m_cnt--;
				}
				this->in_chamber = true;
			}
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
			//
			std::array<frames, 3> LEFTBodyFrame;		//左手座標(マガジン保持時)

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
			bool running = false;		//走るか否か
			bool jamp = false;			//ジャンプ
			bool shoot = false;			//射撃
			bool reload = false;		//リロード開始
			bool have_mag = false;		//マガジンを持つか
			bool have_item = false;		//アイテム取得
			bool sort_magazine = false;	//マガジンソート
			bool view_gun = false;	//マガジンソート
			bool drop_ = false;			//医療品落とす
			bool aim = false;			//エイム
			bool TPS = false;			//TPS
			bool squat = false;			//しゃがむ
			bool select = false;		//セレクター

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
			void UpDate(void) noexcept {
			}
		};
		//VR用の関節
		class Pos_VRJoint {
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts = nullptr;
		public:
			bool oldv_2 = false;
			bool oldv_2_2 = false;
			VECTOR_ref pos_rep;
			VECTOR_ref pos;
			MATRIX_ref mat_rep;
			MATRIX_ref mat;

			auto get_pos_noY() const noexcept { return VECTOR_ref::vget(this->pos.x(), 0.f, this->pos.z()); }
			MATRIX_ref get_mat_XZ() const noexcept { return MATRIX_ref::Axis1(this->mat.xvec()*-1.f, this->mat.yvec(), this->mat.zvec()*-1.f); }

			void Set(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts_t) noexcept {
				DrawPts = DrawPts_t;
				oldv_2 = true;
				oldv_2_2 = true;
			}

			void UpDate_none(const char& device_num) noexcept {
				(*DrawPts)->GetDevicePositionVR(device_num, &this->pos, &this->mat);
				this->mat = get_mat_XZ();
			}

			void UpDate_pos(const char& device_num, const bool flag_start_loop) noexcept {
				auto& ptr_ = (*(*DrawPts)->get_device())[device_num];
				(*DrawPts)->GetDevicePositionVR(device_num, &this->pos, &this->mat);
				if (flag_start_loop || (ptr_.turn && ptr_.now) != this->oldv_2) {
					this->pos_rep = get_pos_noY();
				}
				this->oldv_2 = ptr_.turn && ptr_.now;
			}

			void UpDate_mat(const char& device_num, const bool flag_start_loop) noexcept {
				auto& ptr_ = (*(*DrawPts)->get_device())[device_num];
				UpDate_none(device_num);
				if ((flag_start_loop || (ptr_.turn && ptr_.now) != this->oldv_2) && this->oldv_2_2) {
					this->mat_rep = this->mat;
					if (!flag_start_loop) {
						//this->oldv_2_2 = false;
					}
				}
				this->oldv_2 = ptr_.turn && ptr_.now;
			}
		};
		//スコア
		class score_s {
			float death_timer = 0.f;
		public:
			float kill_timer = 0.f;

			int kill_cnt = 0;
			int death_cnt = 0;
			size_t kill_id = 0;
			size_t death_id = 0;
			bool kill_f = false;
			bool death_f = false;

			int score = 0;
			int kill_streak = 0;
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
		//
		class damage_rad {
		public:
			float alpfa = 1.f;
			float rad = 0.f;
		};
		//
		class anime_arm {
		public:
			float pers = 0.f;
			float total = 1.f;
		};
		class aniv {
		public:
			std::array<VECTOR_ref, 3> vec;		//座標
		};
		//AI用
		class AI{
		public:
			float ai_time_a = 0.f;
			float ai_time_d = 0.f;
			float ai_time_q = 0.f;
			float ai_time_e = 0.f;
			float ai_time_shoot = 0.f;
			float ai_time_walk = 0.f;
			bool ai_reload = false;
			int ai_phase = 0;
			std::array<int, 6> wayp_pre{ 0 };

			void spawn(int now) {
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
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) {
					this->wayp_pre[i] = this->wayp_pre[i - 1];
				}
				this->wayp_pre[0] = now;
			}
		};
	public:
		class g_parts {
		public:
			frames slide_f;
		private:
			size_t type = 0;
			frames rad_parts;
			VECTOR_ref vec_parts;
			bool attach = false;
			//専用
			frames mag_f;
			frames magazine2_f;
			frames cate_f;
			frames site_f;
			frames mazzule_frame;					//マズル
			frames attach_frame;					//根元
			frames source_frame;					//光源
			frames sight_frame;						//サイト
			std::array<frames, 3> LEFT_frame;		//左手座標
			std::array<frames, 3> RIGHT_frame;		//右手座標
			std::array<frames, 2> magazine_f;		//マガジンフレーム
			GraphHandle reticle;					//レティクル
			int LightHandle = -1;					//ライト
		public:
			MV1 obj;
			//MV1 col;
			std::array<MV1, 2> obj_ammo;			//マガジン用弾
			std::array<frames, 3> LEFT_mag_frame;	//左手座標(マガジン保持時)
			std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム
			std::array<std::array<frames, 2>, EnumAttachPoint::NUM_> rail_f;
			GUNPARTs* thisparts = nullptr;
			g_parts* attach_parts = nullptr;
			VECTOR_ref sight_vec;

			auto& get_type() const noexcept { return type; }
			auto& get_attaching() const noexcept { return attach; }
			auto& get_attach_frame() const noexcept { return attach_frame.second; }
			auto& get_sight_frame() const noexcept { return sight_frame.second; }
			const int get_mazzuletype() noexcept {
				if (this->attach) {
					return int(this->thisparts->mazzule_type);
				}
				return -1;
			}
			const int get_stocktype() noexcept {
				if (this->attach) {
					return int(this->thisparts->stock_type);
				}
				return -1;
			}
			const auto mazzule_pos() noexcept {
				if (this->attach) {
					return this->obj.frame(this->mazzule_frame.first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto cate_pos() noexcept {
				if (this->attach) {
					return this->obj.frame(this->cate_f.first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto cate_vec() noexcept {
				return (this->obj.frame(this->cate_f.first + 1) - this->cate_pos()).Norm();
			}
			const auto sight_pos() noexcept {
				if (this->site_f.first != INT_MAX) {
					return this->site_f.second;
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto LEFT_pos(size_t select_t) noexcept {
				if (this->attach) {
					return this->obj.frame(this->LEFT_frame[select_t].first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto LEFT_mag_pos(size_t select_t) noexcept {
				if (this->attach) {
					return this->obj.frame(this->LEFT_mag_frame[select_t].first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto RIGHT_pos(size_t select_t) noexcept {
				if (this->attach) {
					return this->obj.frame(this->RIGHT_frame[select_t].first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto magf_pos() noexcept {
				if (this->attach) {
					return this->obj.frame(this->mag_f.first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			auto& getmagazine_f(int i) const noexcept { return magazine_f[i]; }
			const auto mag2f_pos() noexcept {
				if (this->attach) {
					return this->obj.frame(this->magazine2_f.first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			const auto source_frame_pos() noexcept {
				if (this->attach) {
					return this->obj.frame(this->source_frame.first);
				}
				return VECTOR_ref::vget(0, 0, 0);
			}
			g_parts(void) noexcept {
				this->LightHandle = -1;
			}
			void Draw() noexcept {
				if (this->attach) {
					this->obj.DrawModel();
				}
			}
			void ReSet_frame(void) noexcept {
				for (auto& r : this->rail_f) {
					r[0].first = -1;
					r[1].first = -1;
				}
				rad_parts.first = -1;
				attach_frame.first = -1;

				mag_f.first = INT_MAX;
				magazine2_f.first = INT_MAX;
				site_f.first = INT_MAX;
				cate_f.first = INT_MAX;
			}
			void get_frame_(GUNPARTs& parts, size_t type_t) noexcept {
				if (this->attach) {
					this->thisparts = &parts;
					this->type = type_t;
					{
						this->ReSet_frame();
						SetUseASyncLoadFlag(FALSE);
						if (this->type == EnumGunParts::PARTS_BASE) {
							this->thisparts->mod.get_model().DuplicateonAnime(&this->obj, &this->obj);
						}
						else {
							this->obj = this->thisparts->mod.get_model().Duplicate();
						}

						if (this->type == EnumGunParts::PARTS_MAGAZINE) {
							this->obj_ammo[0] = this->thisparts->ammo[0].get_model_full().Duplicate();
							this->obj_ammo[1] = this->thisparts->ammo[0].get_model_full().Duplicate();
						}
						for (int i = 0; i < int(this->obj.frame_num()); ++i) {
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
								this->mazzule_frame = { int(i),this->obj.frame(i) };
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
								this->LEFT_frame[2] = { int(i + 2),this->obj.GetFrameLocalMatrix(i + 2).pos() };
							}
							if (p == std::string("LEFT_mag")) {
								this->LEFT_mag_frame[0] = { int(i),this->obj.frame(i) };
								this->LEFT_mag_frame[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
								this->LEFT_mag_frame[2] = { int(i + 2),this->obj.GetFrameLocalMatrix(i + 2).pos() };
							}
							if (p == std::string("RIGHT")) {
								this->RIGHT_frame[0] = { int(i),this->obj.frame(i) };
								this->RIGHT_frame[1] = { int(i + 1),this->obj.GetFrameLocalMatrix(i + 1).pos() };
								this->RIGHT_frame[2] = { int(i + 2),this->obj.GetFrameLocalMatrix(i + 2).pos() };
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

							if (p == std::string("mag_fall")) {
								this->mag_f = { i, this->obj.frame(i) };//マガジン
								this->magazine2_f = { i + 1, this->obj.frame(i + 1) };//マガジン
							}
							else if (p == std::string("site")) {
								this->site_f = { i, this->obj.frame(i) };//アイアンサイト
							}
							else if (p == std::string("cate")) {
								this->cate_f = { i, this->obj.frame(i) };//排莢
							}

							if (p == std::string("slide")) {
								this->slide_f = { i, this->obj.frame(i) };//
							}
						}
					}
					switch (this->type) {
					case EnumGunParts::PARTS_SIGHT:
						this->reticle = this->thisparts->reticle.Duplicate();
						this->obj.SetupCollInfo(1, 1, 1, -1, 1);
						break;
					case EnumGunParts::PARTS_LAM:
						if (this->LightHandle != -1) {
							DeleteLightHandle(this->LightHandle);
							this->LightHandle = -1;
						}
						break;
					default:
						break;
					}
				}
			}
			void Draw_reticle() noexcept {
				if (this->attach) {
					if (this->type == EnumGunParts::PARTS_SIGHT) {
						auto ret = this->obj.frame(3);
						auto camp = GetCameraPosition();

						//clsDx();
						//printfDx("%0.5f", (camp - this->obj.frame(this->sight_frame.first)).size());
						if (this->obj.CollCheck_Line(camp, ret, -1, 1).HitFlag == TRUE) {
							SetUseZBuffer3D(FALSE);
							DrawBillboard3D(ret.get(), 0.5f, 0.5f, 3.f, 0.f, this->reticle.get(), TRUE);
							SetUseZBuffer3D(TRUE);
						}
					}
				}
			}
			void Set_reticle(void) noexcept {
				if (this->attach) {
					this->obj.RefreshCollInfo(-1, 1);
				}
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, size_t type_t) noexcept {
				detach_(per_all);
				this->attach = true;
				if (this->attach) {
					this->get_frame_(partsptr, type_t);
					per_all += this->thisparts->per;
				}
			}
			void attach_(performance& per_all, GUNPARTs& partsptr, size_t type_t, g_parts* parents, size_t side) noexcept {
				detach_(per_all);
				this->attach = false;
				if (parents != nullptr) {
					if (parents->thisparts != nullptr) {
						for (auto& p : parents->thisparts->can_attach) {
							if (p == partsptr.mod.get_name()) {
								this->attach = (parents->rail_f[side][0].first > 0);
								if (this->attach) {
									this->get_frame_(partsptr, type_t);
									per_all += this->thisparts->per;

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
			void detach_(performance& per_all) noexcept {
				if (this->attach) {
					this->attach = false;
					this->ReSet_frame();
					per_all -= this->thisparts->per;
					this->thisparts = nullptr;
					this->attach_parts = nullptr;
					this->type = EnumGunParts::PARTS_NONE;
					MV1TerminateCollInfo(this->obj.get(), -1, 1);
					this->obj.Dispose();
					this->reticle.Dispose();
					//
					if (this->LightHandle != -1) {
						DeleteLightHandle(this->LightHandle);
						this->LightHandle = -1;
					}
				}
			}
			void Setpos_Nomal(const MATRIX_ref& mat_t) noexcept {
				if (this->attach) {
					this->obj.SetMatrix(mat_t);//方向指定
				}
			}
			void Setpos_parts(const MATRIX_ref& mat_t) noexcept {
				if (this->attach) {
					auto fpos_t = MATRIX_ref::RotVec2(this->rad_parts.second.Norm(), this->vec_parts)*mat_t;
					this->obj.SetMatrix(fpos_t*MATRIX_ref::Mtrans(this->attach_parts->obj.frame(this->attach_frame.first)));//方向指定
				}
			}
			void Set_LightHandle(const VECTOR_ref& Vec) noexcept {
				if (this->attach && this->thisparts->select_lam[0] == EnumSELECT_LAM::SELECT_LIGHT) {
					auto StartPos = this->source_frame_pos();
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
		//腕アニメ関連
		aniv* prev_ani_lefthand = nullptr;
		aniv* next_ani_lefthand = nullptr;
		anime_arm* now_timeline = nullptr;
		//
		std::vector<anime_arm> ani_timeline;
		std::vector<aniv> ani_lefthand;
		VECTOR_ref vec_gunani = VECTOR_ref::vget(0.f, 0.f, 1.f);
		VECTOR_ref pos_gunani = VECTOR_ref::vget(0.f, 0.f, 0.f);
		float zrad_gunani = 0.f;
		//
		bool have_magazine = true;			//マガジンを持つか
		//
		MV1 obj_body;
		MV1 obj_lag;
		//プレイヤー座標系
		VECTOR_ref pos_tt;			//位置
		MATRIX_ref mat_notlean;		//位置
		VECTOR_ref add_vec;			//移動ベクトル
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<BULLETS, 32> bullet;
		std::array<CARTS, 32> cart;
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
		MV1::ani* anime_jump = nullptr;
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
		bool nearhit = false;
		//プレイヤー座標系
		float speed = 0.f;			//
		MATRIX_ref mat_body;		//位置
		VECTOR_ref spawn_pos;		//spawnpos
		MATRIX_ref spawn_mat;		//spawnmat
		VECTOR_ref add_vec_buf;		//移動ベクトルバッファ
		float xrad_p = 0.f;			//マウスエイム用変数確保
		bool sort_ing = false;		//整頓
		bool view_ing = false;		//整頓
		size_t view_ings = 0;
		size_t reload_ings = 0;
		VECTOR_ref gunpos;			//マウスエイム用銃座標
		bool gunf = false;			//射撃フラグ
		opes key_;					//入力関連
		VECTOR_ref add_vec_real;	//実際の移動ベクトル
		//AI用
		AI cpu_do;
		//マガジン座標系
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//銃座標
		VECTOR_ref pos_gun;
		MATRIX_ref mat_gun;
		VECTOR_ref addpos_gun;
		VECTOR_ref blur_vec, blur_vec_res;
		VECTOR_ref recoil_vec, recoil_vec_res;
		float rad_gun = 0.f;
		//頭部座標系
		Pos_VRJoint HMD;
		VECTOR_ref pos_HMD_old;
		float add_ypos = 0.f;		//垂直加速度
		float body_xrad = 0.f;		//胴体角度
		float body_yrad = 0.f;		//胴体角度
		float body_zrad = 0.f;		//胴体角度
		float body_ztrun = 0.f;		//胴体角度
		//フレーム
		frame_body frame_s;
		frame_body lagframe_;
		frame_body colframe_;
		Pos_VRJoint RIGHTHAND;		//右手座標系
		Pos_VRJoint LEFTHAND;		//左手座標系
		Pos_VRJoint RIGHTREG;		//右脚座標系
		Pos_VRJoint LEFTREG;		//左脚座標系
		Pos_VRJoint WAIST;			//腰座標
		//
		bool flag_canlook_player = true;
		bool flag_calc_body = true;
		bool flag_start_loop = true;
		bool flag_calc_lag = true;
		//
		float distance_to_cam = -1.f;
		//
		float ratio_r = 0.f;
		//性能
		performance per_all;
		//取得可能アイテムID
		size_t canget_id = 0;
		//取得可能アイテム(マガジン)
		bool canget_magitem_f = false;
		std::string canget_mag;
		//取得可能アイテム(衣料品)
		bool canget_meditem_f = false;
		std::string canget_med;
		//
		float zpos_cart = 0.f;
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
	public:
		/*所持弾数などの武器データ*/
		GUN_STATUS* gun_stat_now = nullptr;
		std::vector<g_parts> sight_;
		//HPバー
		unsigned int got_damage_color = 0;
		int got_damage_x = 0;
		float got_damage_f = 0.f;
		std::vector<damage_rad> got_damage_;
		int got_damage = 0;
		int HP = 100;
		float HP_r = 100;
		int HP_full = 100;
		std::vector<int> HP_parts;
		//スコア
		score_s scores;
		//
		bool sort_f = false;
		bool reloadf = false;

	public:
		//
		g_parts* get_parts(size_t type_sel, int sight_mount = 0) noexcept {
			switch (type_sel) {
			case EnumGunParts::PARTS_BASE:
				return &this->base;
			case EnumGunParts::PARTS_MAGAZINE:
				return &this->magazine;
			case EnumGunParts::PARTS_GRIP:
				return &this->grip;
			case EnumGunParts::PARTS_UPER_HGUARD:
				return &this->uperhandguard;
			case EnumGunParts::PARTS_UNDER_HGUARD:
				return &this->underhandguard;
			case EnumGunParts::PARTS_MAZZULE:
				return &this->mazzule;
			case EnumGunParts::PARTS_DUSTCOVER:
				return &this->dustcover;
			case EnumGunParts::PARTS_STOCK:
				return &this->stock;
			case EnumGunParts::PARTS_LAM:
				return &this->lam;
			case EnumGunParts::PARTS_FOREGRIP:
				return &this->foregrip;
			case EnumGunParts::PARTS_MOUNT_BASE:
				return &this->mount_base;
			case EnumGunParts::PARTS_MOUNT:
				return &this->mount_;
			case EnumGunParts::PARTS_SIGHT:
				return &this->sight_[sight_mount];
			default:
				break;
			}
			return nullptr;
		}
		auto* get_mag_g_parts(void) noexcept { return get_parts(EnumGunParts::PARTS_MAGAZINE); }
		auto get_maz(void) noexcept {
			if (mazzule.get_mazzuletype() > 0) {
				return mazzule.mazzule_pos();
			}
			else {
				return base.mazzule_pos();
			}
		}
		auto get_now_selector(void) noexcept { return this->get_parts(EnumGunParts::PARTS_BASE)->thisparts->select[this->gun_stat_now->selecter]; }
		auto& get_mat_gun(void) noexcept { return mat_gun; }
		//視点取得
		void set_HMDpos() noexcept {
			this->HMD.pos = (this->BodyFrame(this->frame_s.RIGHTeye_f.first) + (this->BodyFrame(this->frame_s.LEFTeye_f.first) - this->BodyFrame(this->frame_s.RIGHTeye_f.first))*0.5f) - this->pos_tt;
		}
		//ミニマップ用
		const float get_body_yrad(void) const noexcept { return this->body_yrad; }
		//生きているか
		const bool get_alive(void) const noexcept { return this->HP != 0; }
		//pos
		const VECTOR_ref& get_pos(void) noexcept { return this->pos_tt + this->HMD.pos - this->HMD.pos_rep; }
		//UI表示用
		const auto& get_per_all(void) const noexcept { return per_all; }		//性能
		//key
		auto& get_key_(void) noexcept { return key_; }
		//ADS中
		const bool ads_on(void) const noexcept { return this->key_.aim && !this->view_ing; }
		//ジャンプ中
		const bool get_jamping(void) const noexcept { return this->add_ypos!=0.f; }
		//gunfをOFFからONにする
		const bool set_gunf(void) noexcept {
			if (!this->gunf) {
				this->gunf = true;
				return true;
			}
			return false;
		}
		//移動速度のパーセンテージ
		const float get_pseed_per(void) const noexcept { return add_vec_real.size() / speed; }
		//item関連
		const bool& getmagazine_push(void) const noexcept { return this->key_.have_item; }
		const auto& get_pos_LEFTHAND(void) const noexcept { return LEFTHAND.pos; }
		const auto& get_mat_LEFTHAND(void) const noexcept { return LEFTHAND.mat; }
		const auto& get_canget_mag_f(void)const  noexcept { return canget_magitem_f; }
		const auto& get_canget_med_f(void)const  noexcept { return canget_meditem_f; }
		const auto& get_canget_mag_s(void) const noexcept { return canget_mag; }
		const auto& get_canget_med(void) const noexcept { return canget_med; }
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
		const VECTOR_ref& BodyFrame(const int& p1) const noexcept { return obj_body.frame(p1); }
		const MATRIX_ref& BodyFrameMatrix(const int& id) const noexcept { return obj_body.GetFrameLocalWorldMatrix(id); }
	public:
		//頭部座標
		const auto get_head_pos(void) const noexcept { return this->BodyFrame(this->frame_s.head_f.first); }		//TPS
	private:
		//右腕座標(固定)
		const auto& RIGHT_pos_gun(size_t select_t) noexcept { return this->base.RIGHT_pos(select_t); }
		//左腕座標
		const auto& LEFT_pos_gun(size_t select_t) noexcept {
			auto ans = this->underhandguard.LEFT_pos(select_t);
			auto f_ = this->foregrip.LEFT_pos(select_t);
			if (f_ != VECTOR_ref::vget(0, 0, 0)) { ans = f_; }
			return ans;
		}
		const auto& LEFT_mag_pos_gun(size_t select_t) noexcept { return this->base.LEFT_mag_pos(select_t); }
		const auto& LEFT_body_pos_gun(size_t select_t)const  noexcept { return this->BodyFrame(this->frame_s.LEFTBodyFrame[select_t].first); }
	private:
		//腕アニメで使う座標の決定
		void Set_GUN_pos_Anim() noexcept {
			for (int i = 0; i < 3; ++i) {
				ani_lefthand[0].vec[i] = this->LEFT_pos_gun(i);
				ani_lefthand[1].vec[i] = this->LEFT_mag_pos_gun(i);
				ani_lefthand[2].vec[i] = this->LEFT_body_pos_gun(i);
			}
		}
		//腕アニメの指定関数
		bool Func_Set_LEFT_pos_Anim(anime_arm*anim, aniv*next_ani, const VECTOR_ref& vec_gun, const VECTOR_ref& pos_gun_t, float zrad_gun) noexcept {
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
		//腕アニメを参照
		const auto LEFT_pos_Anim(size_t select_t) noexcept {
			if (now_timeline != nullptr) {
				if (now_timeline->pers < now_timeline->total) {
					auto pers = now_timeline->pers / now_timeline->total;
					return prev_ani_lefthand->vec[select_t] * (1.f - pers) + next_ani_lefthand->vec[select_t] * pers;
				}
				else {
					return next_ani_lefthand->vec[select_t];
				}
			}
			else {
				return next_ani_lefthand->vec[select_t];
			}
		}
		//腕アニメの決定
		void Set_LEFT_pos_Anim() noexcept {
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

			this->ani_timeline[11].total = 0.4f;//構える
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
					//眺める
					if (this->view_ing) {
						switch (this->view_ings) {
						case 0:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[7], &this->ani_lefthand[0], VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -65.f)) {
								this->view_ings++;
							}
							return;
						}
						case 1:
						{
							if (Func_Set_LEFT_pos_Anim(&this->ani_timeline[8], &this->ani_lefthand[0], VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -65.f)) {
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
		//リロード中か否か
		bool isReload(void) const noexcept { return this->reloadf && this->gun_stat_now->hav_mags(); }//リロード中
		//アイテムリリース
		void magrelease(MAG_STATUS& mag) noexcept {
			auto& item = (*MAPPTs)->item;
			auto add_vec_t = (this->base.magf_pos() - this->base.mag2f_pos()).Norm()*1.f / FPS;
			for (auto i = item.begin(); i != item.end();) {
				if (i->Set_item_magrelease(mag.ptr_mag, this->pos_mag, add_vec_t, this->mat_mag, mag.m_cnt)) {
					break;
				}
				++i;
				if (i == item.end()) {
					item.resize(item.size() + 1);
					item.back().Set_item_magazine(item.size() - 1, mag.ptr_mag, this->pos_mag, add_vec_t, this->mat_mag, mag.m_cnt);
				}
			}
		}
		void medrelease(Meds*meddata) noexcept {
			auto& item = (*MAPPTs)->item;
			auto add_vec_t = (this->base.obj.GetMatrix().zvec()).Norm()*-5.f / FPS;
			for (auto i = item.begin(); i != item.end();) {
				if (i->Set_item_med(meddata, this->get_maz(), add_vec_t, this->mat_gun)) {
					break;
				}
				++i;
				if (i == item.end()) {
					item.resize(item.size() + 1);
					item.back().Set_item_med_(item.size() - 1, meddata, this->get_maz(), add_vec_t, this->mat_gun);
				}
			}
		}
		//マガジン排出
		void magrelease_t(size_t test_) noexcept {
			if (this->gun_stat_now->hav_mags()) {
				//音
				this->audio.magazine_down.play_3D(this->pos_gun, 15.f);
				//弾数
				if (test_ == 0) {//全排出
					while (true) {
						magrelease(this->gun_stat_now->get_magazine_in().front());//アイテムを落とす
						this->gun_stat_now->get_magazine_in().erase(this->gun_stat_now->get_magazine_in().begin());//先頭削除
						if (!this->gun_stat_now->hav_mags()) { break; }
					}
				}
				if (test_ == 1) {//通常排出
					magrelease(this->gun_stat_now->get_magazine_in().front());//アイテムを落とす
					this->gun_stat_now->get_magazine_in().erase(this->gun_stat_now->get_magazine_in().begin());//先頭削除
				}

				if (test_ == 2) {//末尾排出
					magrelease(this->gun_stat_now->get_magazine_in().back());//アイテムを落とす
					this->gun_stat_now->get_magazine_in().pop_back();//末尾削除
				}
				this->reload_cnt = 0.f;
				return;
			}
		}
		//サイトの親の座標を探す
		void search_sight_base(VECTOR_ref* pv, const g_parts& ptr) noexcept {
			if (ptr.attach_parts != &this->base && ptr.attach_parts != nullptr) {
				*pv += ptr.attach_parts->get_attach_frame();
				search_sight_base(pv, *ptr.attach_parts);
			}
		}
		//腕のフレーム操作
		void move_hand(const bool isusewaist, const MATRIX_ref& m_inv, bool ptt, bool LorR) noexcept {
			//左
			if (LorR) {
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((this->LEFTHAND.pos - this->BodyFrame(this->frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFThand_f.first) - this->BodyFrame(this->frame_s.LEFTarm2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTarm2_f.first) - this->BodyFrame(this->frame_s.LEFTarm1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTarm1_f.first) - this->LEFTHAND.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((ptt && !isusewaist) || !ptt) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
				}
				if (ptt && isusewaist) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, (this->WAIST.mat*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTarm2_f.first) - this->BodyFrame(this->frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((ptt && !isusewaist) || !ptt) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
				}
				if (ptt && isusewaist) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm1_f.first, a1_inv*(this->WAIST.mat*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTarm1_f.second));
				}
				//下腕
				this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFThand_f.first) - this->BodyFrame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->LEFTHAND.pos - this->BodyFrame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTarm2_f.second));
				//手
				this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFThand_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFThand_f.second));
				auto ans1_ = this->LEFT_pos_Anim(0);
				auto ans2_ = this->LEFT_pos_Anim(1);
				auto ans3_ = this->LEFT_pos_Anim(2);
				MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFThand2_f.first) - this->BodyFrame(this->frame_s.LEFThand_f.first), m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()),
					MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFThand_f.first, a3_inv * MATRIX_ref::Mtrans(this->frame_s.LEFThand_f.second));
				MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrameMatrix(this->frame_s.LEFThand_f.first).zvec()*-1.f, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()),
					MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFThand_f.first, a3_yvec * a3_inv * MATRIX_ref::Mtrans(this->frame_s.LEFThand_f.second));
			}
			//右
			else {
				//基準
				this->RIGHTHAND.pos = this->RIGHT_pos_gun(0);
				if (!ptt) {
					this->RIGHTHAND.mat = this->mat_gun;
				}
				auto vec_a1 = MATRIX_ref::Vtrans((this->RIGHTHAND.pos - this->BodyFrame(this->frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(-1.f*sin(deg2rad(30)), -1.f*cos(deg2rad(30)), vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHThand_f.first) - this->BodyFrame(this->frame_s.RIGHTarm2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTarm2_f.first) - this->BodyFrame(this->frame_s.RIGHTarm1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTarm1_f.first) - this->RIGHTHAND.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((ptt && !isusewaist) || !ptt) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
				}
				if (ptt && isusewaist) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, (this->WAIST.mat*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTarm2_f.first) - this->BodyFrame(this->frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((ptt && !isusewaist) || !ptt) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
				}
				if (ptt && isusewaist) {
					this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm1_f.first, a1_inv*(this->WAIST.mat*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm1_f.second));
				}
				//下腕
				this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHThand_f.first) - this->BodyFrame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(this->RIGHTHAND.pos - this->BodyFrame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTarm2_f.second));
				//手
				this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHThand_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHThand_f.second));
				auto ans1_ = this->RIGHT_pos_gun(0);
				auto ans2_ = this->RIGHT_pos_gun(1);
				auto ans3_ = this->RIGHT_pos_gun(2);
				MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHThand2_f.first) - this->BodyFrame(this->frame_s.RIGHThand_f.first), m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()),
					MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHThand_f.first, a3_inv * MATRIX_ref::Mtrans(this->frame_s.RIGHThand_f.second));
				MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(this->BodyFrameMatrix(this->frame_s.RIGHThand_f.first).zvec()*-1.f, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()),
					MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*a3_inv.Inverse()));
				this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHThand_f.first, a3_yvec * a3_inv * MATRIX_ref::Mtrans(this->frame_s.RIGHThand_f.second));
			}
		}
		//手振れ
		MATRIX_ref get_res_blur(float persent) noexcept {
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
		const float Head_bobbing(MV1::ani* ani_ptr) noexcept { return sin(DX_PI_F*2.f*(ani_ptr->time / ani_ptr->alltime)) *ani_ptr->per; }
		//銃器の位置指定(右手が沿うため)
		void move_nomal_gun(Chara& mine) noexcept {
			{
				//gunpos指定
				{
					//サイト位置決定
					VECTOR_ref sight_vec = this->base.sight_pos();//アイアンサイト
					for (auto&s : this->sight_) {
						if (s.get_attaching()) {
							sight_vec = s.sight_vec;//サイト
						}
					}
					//gunpos指定
					if (this->ads_on()) {
						easing_set(&this->gunpos,
							VECTOR_ref::vget(-0.035f - sight_vec.x(), -sight_vec.y() + Head_bobbing(this->anime_walk)*0.001f, -0.15f),
							std::min(0.75f + ((0.9f - 0.75f)*(this->per_all.weight - this->base.thisparts->per.weight) / 3.f), 0.935f));
					}
					else {
						easing_set(&this->gunpos,
							VECTOR_ref::vget(-0.15f - sight_vec.x(), -0.07f - sight_vec.y() + Head_bobbing(this->anime_walk)*0.002f, ((this == &mine) ? -0.09f : -0.15f)),
							0.75f);
					}
				}
				//視点を一時取得(get_pos対処)
				set_HMDpos();
				//通常
				this->mat_gun = get_res_blur(1.f)*this->HMD.mat;//リコイル
				this->pos_gun = this->get_pos() + (MATRIX_ref::Vtrans(this->gunpos, this->mat_gun) - this->HMD.pos_rep);
				//リロード
				set_gun_pos(this->pos_gun + MATRIX_ref::Vtrans(pos_gunani, this->mat_gun), MATRIX_ref::RotZ(deg2rad(zrad_gunani))* MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, 1.f), vec_gunani)* this->mat_gun);
				//壁沿い
				{
					int radd = 0;
					MATRIX_ref mat_gun_old = this->mat_gun;
					while (true) {
						this->base.obj.SetMatrix(this->Gun_Mat());
						this->mazzule.Setpos_parts(this->mat_gun);//一時的
						if ((*MAPPTs)->map_col_line(this->RIGHT_pos_gun(0), this->get_maz()).HitFlag) {
							set_gun_pos(this->pos_gun + mat_gun_old.zvec()*0.006f, MATRIX_ref::RotX(deg2rad(2))* this->mat_gun);//ひっこめ(仮)
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
					set_gun_pos(this->pos_gun, MATRIX_ref::RotX(deg2rad(this->rad_gun))* mat_gun_old);//ひっこめ
				}
				this->Set_gun();//一時的に銃位置更新
			}
		}
		//自分についているパーツがある場合削除
		void Detach_child(g_parts* parent, size_t type_t, int sight_s = 0) noexcept {
			if (parent != nullptr) {
				//
				if (this->mount_base.attach_parts == parent) { this->mount_base.detach_(this->per_all); }
				if (this->mount_.attach_parts == parent) { this->mount_.detach_(this->per_all); }
				for (auto& s : this->sight_) { if (s.attach_parts == parent) { s.detach_(this->per_all); } }
				if (this->mazzule.attach_parts == parent) { this->mazzule.detach_(this->per_all); }
				if (this->grip.attach_parts == parent) { this->grip.detach_(this->per_all); }
				if (this->uperhandguard.attach_parts == parent) { this->uperhandguard.detach_(this->per_all); }
				if (this->underhandguard.attach_parts == parent) { this->underhandguard.detach_(this->per_all); }
				//
				if (this->dustcover.attach_parts == parent) { this->dustcover.detach_(this->per_all); }
				if (this->stock.attach_parts == parent) { this->stock.detach_(this->per_all); }
				if (this->foregrip.attach_parts == parent) { this->foregrip.detach_(this->per_all); }
				if (this->lam.attach_parts == parent) { this->lam.detach_(this->per_all); }
				//
				switch (type_t) {
				case EnumGunParts::PARTS_SIGHT:
				{
					if (this->sight_.size() > sight_s) { this->sight_[sight_s].detach_(this->per_all); }
					break;
				}
				default:
					parent->detach_(this->per_all);
					break;
				}
			}
		}
		//VR操作
		void vr_move(void) {
			if ((*DrawPts)->get_hand2_num() != -1) {
				auto& ptr_ = (*(*DrawPts)->get_device())[(*DrawPts)->get_hand2_num()];
				if (ptr_.turn && ptr_.now) {
					//移動
					if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
						this->speed = (this->key_.running ? 8.f : 6.f) / FPS;

						if ((*DrawPts)->tracker_num.size() > 0) {
							auto p = this->BodyFrameMatrix(this->frame_s.bodyb_f.first);
							easing_set(&this->add_vec_buf, (p.zvec()*-ptr_.touch.y() + p.xvec()*-ptr_.touch.x())*this->speed, 0.95f);
						}
						else {
							easing_set(&this->add_vec_buf, (this->HMD.mat.zvec()*ptr_.touch.y() + this->HMD.mat.xvec()*ptr_.touch.x())*this->speed, 0.95f);
						}
					}
					else {
						easing_set(&this->add_vec_buf, VECTOR_ref::vget(0, 0, 0), 0.95f);
					}
				}
			}
		}
		//Nomal操作
		void key_move(int32_t& x_m, int32_t& y_m, const float fov_per) {
			GetMousePoint(&x_m, &y_m);//~0.01
			x_m = int(float(std::clamp(x_m - deskx / 2, -120, 120))*fov_per);
			y_m = int(float(std::clamp(y_m - desky / 2, -120, 120))*fov_per);
			SetMousePoint(deskx / 2, desky / 2);//ウィンドウだと0.05〜0.10ms掛かる?!
			SetMouseDispFlag(FALSE);
		}
		//AI操作
		void AI_move(int32_t& x_m, int32_t& y_m, std::vector<Chara>&chara) {
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
				//this->qkey = false;
				//this->ekey = false;
				this->key_.aim = false;
				//this->TPS = false;
				this->key_.reload = false;
				this->key_.have_item = false;
				this->key_.sort_magazine = false;
				this->key_.view_gun = false;
				this->key_.drop_ = false;
				this->key_.select = false;
				this->key_.have_mag = true;
				this->key_.jamp = false;
				this->key_.squat = false;
			}
			//
			VECTOR_ref vec_to;
			//
			auto poss = this->get_pos();
			auto& waypoint = (*MAPPTs)->get_waypoint();
			auto vec_mat = this->BodyFrameMatrix(this->frame_s.head_f.first);
			auto vec_x = vec_mat.xvec();
			auto vec_y = vec_mat.yvec();
			auto vec_z = vec_mat.zvec()*-1.f;
			{
				auto ai_p_old = this->cpu_do.ai_phase;
				auto StartPos = this->get_head_pos();
				bool pp = true;
				for (auto& tgt : chara) {
					if (this == &tgt) {
						continue;
					}
					if (tgt.gunf) { turn = int(&tgt - &chara[0]); }
					auto EndPos = (this->cpu_do.ai_time_shoot < 0.f) ? tgt.BodyFrame(tgt.frame_s.head_f.first) : tgt.BodyFrame(tgt.frame_s.bodyb_f.first);
					if (&tgt == &chara[0]) {
						vec_to = EndPos - StartPos;
					}
					{
						if (!tgt.get_alive()) { continue; }
						if ((*MAPPTs)->map_col_line(StartPos, EndPos).HitFlag) { continue; }
						EndPos = EndPos - StartPos;
						if (vec_to.size() >= EndPos.size()) {
							vec_to = EndPos;
							is_player = (&tgt == &chara[0]);
							pp = false;
						}
					}
				}
				if (pp) {
					this->cpu_do.ai_phase = 0;
				}
				else {
					if (vec_z.dot(vec_to.Norm()) >= 0 && vec_to.size() <= 35.f) {
						this->cpu_do.ai_phase = 1;
					}
				}
				if (isReload()) {
					this->cpu_do.ai_phase = 2;
				}
				if ((ai_p_old == 1 && this->cpu_do.ai_phase != 1) || (this->add_vec_real.size() <= this->add_vec.size()*0.8f)) {
					int now = (*MAPPTs)->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
			}
			//
			switch (this->cpu_do.ai_phase) {
			case 0://通常フェイズ
			{
				this->cpu_do.ai_reload = false;

				this->key_.akey = false;
				this->key_.dkey = false;
				//this->key_.qkey = false;
				//this->key_.ekey = false;
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
				vec_to = ((turn >= 0) ? chara[turn].get_pos() : waypoint[this->cpu_do.wayp_pre[0]]) - poss;
				//到達時に判断
				if (vec_to.size() <= 0.5f) {
					int now = (*MAPPTs)->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
				x_m = -int(vec_x.dot(vec_to) * 120);
				y_m = -int(vec_y.dot(vec_to) * 10);
			}
			break;
			case 1://戦闘フェイズ
			{
				this->cpu_do.ai_reload = false;
				/*
				if (this->key_.ekey) {
					this->ai_time_e += 1.f / FPS;
					if (this->ai_time_e >= 2) {
						this->key_.ekey = false;
						this->ai_time_e = 0.f;
					}
				}
				for (auto& w : (*MAPPTs)->get_leanpoint_e()) {
					if ((w - poss).size() <= 0.5f) {
						this->key_.ekey = true;
						this->key_.qkey = false;
						this->ai_time_e = 0.f;
						break;
					}
				}
				if (this->key_.qkey) {
					this->ai_time_q += 1.f / FPS;
					if (this->ai_time_q >= 2) {
						this->key_.qkey = false;
						this->ai_time_q = 0.f;
					}
				}
				for (auto& w : (*MAPPTs)->get_leanpoint_q()) {
					if ((w - poss).size() <= 0.5f) {
						this->key_.ekey = false;
						this->key_.qkey = true;
						this->ai_time_q = 0.f;
						break;
					}
				}
				*/
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
				//this->key_.qkey = false;
				//this->key_.ekey = false;

				this->key_.wkey = true;
				this->key_.running = true;

				if (this->cpu_do.ai_reload) {
					auto ppp = this->cpu_do.wayp_pre[1];
					for (int i = int(this->cpu_do.wayp_pre.size()) - 1; i >= 1; i--) {
						this->cpu_do.wayp_pre[i] = this->cpu_do.wayp_pre[0];
					}
					this->cpu_do.wayp_pre[0] = ppp;
					this->cpu_do.ai_reload = false;
				}
				//向き指定
				vec_to = waypoint[this->cpu_do.wayp_pre[0]] - poss;
				//到達時
				if (vec_to.size() <= 0.5f) {
					int now = (*MAPPTs)->get_next_waypoint(this->cpu_do.wayp_pre, poss);
					if (now != -1) {
						this->cpu_do.set_wayp_pre(now);
					}
				}
				x_m = -int(vec_x.dot(vec_to) * 40);
				y_m = -int(vec_y.dot(vec_to) * 40);
			}
			break;
			default:
				break;
			}
			//y〜z 0.00ms
			//(*DebugPTs)->end_way();
			x_m = std::clamp(x_m, -40, 40);
			y_m = std::clamp(y_m, -40, 40);
		}
	private:
		//操作
		void operation(const bool cannotmove, std::vector<Chara>&chara, const float fov_per, uint8_t move_mode, bool use_vr) noexcept {
			int32_t x_m = 0, y_m = 0;
			if (this->get_alive()) {
				if (this->get_alive()) {
					switch (move_mode) {
					case 0://player move
						if (use_vr) {
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
			if (use_vr) {
				//HMD_mat+視点取得
				this->pos_HMD_old = this->HMD.pos;
				this->HMD.UpDate_pos((*DrawPts)->get_hmd_num(), flag_start_loop);
			}
			else {
				this->key_.rule_();
				if (this->get_alive()) {
					//z軸回転(リーン)
					easing_set(&this->body_ztrun, -deg2rad(25 * x_m / 120)*1.f, 0.9f);
					if (this->key_.qkey) {
						easing_set(&this->body_zrad, deg2rad(-30), 0.9f);
					}
					else if (this->key_.ekey) {
						easing_set(&this->body_zrad, deg2rad(30), 0.9f);
					}
					else {
						easing_set(&this->body_zrad, this->body_ztrun, 0.8f);
					}
					//y軸回転(旋回)
					this->mat_notlean = MATRIX_ref::RotX(-this->xrad_p)*this->mat_notlean;
					this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m)*0.1f, deg2rad(-80), deg2rad(60));
					this->mat_notlean *= MATRIX_ref::RotY(deg2rad(x_m)*0.1f);
					//x軸回転(仰俯)
					this->mat_notlean = MATRIX_ref::RotX(this->xrad_p)*this->mat_notlean;
					//anser
					this->HMD.mat = this->mat_notlean * MATRIX_ref::RotAxis(this->mat_notlean.zvec(), this->body_zrad);
				}
				this->speed = (this->key_.running ? 6.f : ((this->ads_on() ? 2.f : 4.f)*(this->key_.squat ? 0.7f : 1.f))) / FPS;
				auto zv_t = this->HMD.mat.zvec();
				zv_t.y(0.f);
				zv_t = zv_t.Norm();
				auto xv_t = this->HMD.mat.xvec();
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
				this->mat_body = this->HMD.mat;
			}
			//ジャンプ
			{
				if (!this->get_jamping()) {
					if (this->key_.jamp && this->get_alive()) {
						this->add_ypos = 0.05f*FRAME_RATE / FPS;
					}
					this->add_vec = this->add_vec_buf;
				}
				else {
					easing_set(&this->add_vec, VECTOR_ref::vget(0, 0, 0), 0.995f);
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
				//その他
				this->key_.UpDate();
			}
			//
		}
		//壁、床判定
		void wall_col(Chara& mine, const bool use_vr) noexcept {
			//VR用
			if (use_vr) {
				auto pos_t2 = this->pos_tt + (VECTOR_ref(VECTOR_ref::vget(this->pos_HMD_old.x(), 0.f, this->pos_HMD_old.z())) - this->HMD.pos_rep);
				auto pos_t = this->pos_tt + (this->HMD.get_pos_noY() - this->HMD.pos_rep);
				(*MAPPTs)->map_col_wall(pos_t2, &pos_t);//壁
				this->pos_tt = pos_t - (this->HMD.get_pos_noY() - this->HMD.pos_rep);
			}
			//共通
			/*
			{
				auto pos_t2 = this->pos_tt + get_pos_noY() - this->HMD.pos_rep;
				auto pos_t = pos_t2 + (this->add_vec / 2);
				(*MAPPTs)->map_col_wall(pos_t2, &pos_t);//壁
				this->pos_tt = pos_t  - get_pos_noY() + this->HMD.pos_rep;
			}
			//*/
			{
				auto pos_t2 = (this->pos_tt - this->HMD.pos_rep);
				//auto pos_t = pos_t2 + (this->add_vec / 2);
				auto pos_t = pos_t2 + this->add_vec;
				(*MAPPTs)->map_col_wall(pos_t2, &pos_t);//壁
				//落下
				{
					auto pp = (*MAPPTs)->map_col_line(pos_t + VECTOR_ref::vget(0, 1.8f, 0), pos_t + VECTOR_ref::vget(0, -0.01f, 0));
					if (this->add_ypos <= 0.f && pp.HitFlag) {
						pos_t = pp.HitPosition;
						this->add_ypos = 0.f;
					}
					else {
						pos_t.yadd(this->add_ypos);
						this->add_ypos += M_GR / std::powf(FPS, 2.f);
						//復帰
						if (pos_t.y() <= -5.f) {
							pos_t = this->spawn_pos;
							if (!use_vr && (this == &mine)) {
								this->xrad_p = 0;
							}
							this->Spawn(this->spawn_pos, this->spawn_mat);
						}
					}
				}
				//反映
				this->pos_tt = pos_t + this->HMD.pos_rep;
				this->add_vec_real = pos_t - pos_t2;
			}
		}
	private:
		//体の動作を決定
		void Set_body(bool use_vr, Chara& mine) noexcept {
			bool useVR = use_vr && (this == &mine);
			//
			if (this->get_alive()) {
				if (useVR) {
					{
						auto v_ = this->HMD.mat.zvec();
						if ((*DrawPts)->tracker_num.size() > 0) {
							v_ = this->WAIST.mat.zvec();
						}
						float x_1 = -sinf(this->body_yrad);
						float y_1 = cosf(this->body_yrad);
						float x_2 = v_.x();
						float y_2 = -v_.z();
						this->body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / FPS / 10.f;
					}
				}
				else {
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
				//身体,頭部,腰
				MATRIX_ref m_inv = MATRIX_ref::RotY(((*DrawPts)->tracker_num.size() > 0) ? DX_PI_F : 0 + this->body_yrad);
				{
					if (this->get_alive()) {
						this->obj_body.SetMatrix(m_inv);
						if ((*DrawPts)->tracker_num.size() > 0) {
							//腰
							this->obj_body.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, (this->WAIST.mat*m_inv.Inverse())*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
							//頭部
							this->obj_body.SetFrameLocalMatrix(this->frame_s.head_f.first, (this->HMD.get_mat_XZ() *m_inv.Inverse()*(this->WAIST.mat*m_inv.Inverse()).Inverse())
								*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
						}
						else {
							//頭部
							this->obj_body.SetFrameLocalMatrix(this->frame_s.head_f.first, (this->HMD.get_mat_XZ() *m_inv.Inverse())
								*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
						}
						this->obj_body.SetMatrix(m_inv *MATRIX_ref::Mtrans(this->get_pos() - (this->BodyFrame(this->frame_s.RIGHTeye_f.first) + (this->BodyFrame(this->frame_s.LEFTeye_f.first) - this->BodyFrame(this->frame_s.RIGHTeye_f.first))*0.5f)));
					}
					else {
						m_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
						this->obj_body.SetMatrix(m_inv*MATRIX_ref::Mtrans(this->pos_tt + this->HMD.get_pos_noY() - this->HMD.pos_rep));
					}
				}
				//脚関連
				//手関連
				if (this->get_alive()) {
					//右手
					this->RIGHTHAND.UpDate_none((*DrawPts)->get_hand1_num());
					this->RIGHTHAND.mat = this->RIGHTHAND.mat*MATRIX_ref::RotAxis(this->RIGHTHAND.mat.xvec(), deg2rad(-60));
					//左手
					this->LEFTHAND.UpDate_none((*DrawPts)->get_hand2_num());
					this->LEFTHAND.mat *= MATRIX_ref::RotAxis(this->LEFTHAND.mat.xvec(), deg2rad(-60));

					this->RIGHTHAND.mat = get_res_blur(1.f) * this->RIGHTHAND.mat;//リコイル
				}
				{}
				{
					//頭部
					//足
					{
						//左
						if ((*DrawPts)->tracker_num.size() > 1) {

							LEFTREG.UpDate_mat((*DrawPts)->tracker_num[1], flag_start_loop);
							this->LEFTREG.mat = MATRIX_ref::RotY(deg2rad(90 + 60 - 10))* this->LEFTREG.mat_rep.Inverse()*this->LEFTREG.mat;
							this->LEFTREG.pos = this->LEFTREG.pos + (this->pos_tt - this->HMD.pos_rep);
							{
								//基準
								auto tgt_pt = this->LEFTREG.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->LEFTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

								//下腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
								//手
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->LEFTREG.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
							}

							{
								/*
								auto pp = (*MAPPTs)->map_col_line(this->BodyFrame(this->frame_s.LEFTreg2_f.first) + VECTOR_ref::vget(0, 1.8f, 0), this->BodyFrame(this->frame_s.LEFTreg2_f.first));
								if (pp.HitFlag) {
									this->LEFTREG.pos = VECTOR_ref(pp.HitPosition) - (this->BodyFrame(this->frame_s.LEFTreg2_f.first) - this->BodyFrame(this->frame_s.LEFTreg_f.first));
								}
								//*/
							}

							{
								//基準
								auto tgt_pt = this->LEFTREG.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->LEFTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTfoot2_f.first) - this->BodyFrame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot1_f.second));

								//下腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.LEFTreg_f.first) - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.LEFTfoot2_f.second));
								//手
								this->obj_body.SetFrameLocalMatrix(this->frame_s.LEFTreg_f.first, this->LEFTREG.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.LEFTreg_f.second));
							}
						}
						//右
						if ((*DrawPts)->tracker_num.size() > 2) {
							this->RIGHTREG.UpDate_mat((*DrawPts)->tracker_num[2], flag_start_loop);

							this->RIGHTREG.mat = MATRIX_ref::RotY(deg2rad(180 - 22 - 10))* this->RIGHTREG.mat_rep.Inverse()*this->RIGHTREG.mat;
							this->RIGHTREG.pos = this->RIGHTREG.pos + (this->pos_tt - this->HMD.pos_rep);
							{
								//基準
								auto tgt_pt = this->RIGHTREG.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->RIGHTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする


								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
								//下腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
								//手
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->RIGHTREG.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
							}

							{
								/*
								auto pp = (*MAPPTs)->map_col_line(this->BodyFrame(this->frame_s.RIGHTreg2_f.first) + VECTOR_ref::vget(0, 1.8f, 0), this->BodyFrame(this->frame_s.RIGHTreg2_f.first));
								if (pp.HitFlag) {
									this->RIGHTREG.pos = VECTOR_ref(pp.HitPosition) - (this->BodyFrame(this->frame_s.RIGHTreg2_f.first) - this->BodyFrame(this->frame_s.RIGHTreg_f.first));
								}
								*/
							}
							{
								//基準
								auto tgt_pt = this->RIGHTREG.pos;
								auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
								//auto vec_a1L1 = (this->RIGHTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

								auto vec_a1L1 = VECTOR_ref::vget(0, 0, -1.f);

								float cos_t = getcos_tri((this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first)).size(), (this->BodyFrame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
								auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTfoot2_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot1_f.second));
								//下腕
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(this->BodyFrame(this->frame_s.RIGHTreg_f.first) - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - this->BodyFrame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(this->frame_s.RIGHTfoot2_f.second));
								//手
								this->obj_body.SetFrameLocalMatrix(this->frame_s.RIGHTreg_f.first, this->RIGHTREG.mat* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.RIGHTreg_f.second));
							}
						}
					}
					//手+銃器
					{}
					{
						//銃器の一時的な位置指定(右手が沿うため)
						if (this->get_alive()) { this->set_gun_pos(this->RIGHTHAND.pos, this->RIGHTHAND.mat); }
						this->Set_gun();
						this->Set_GUN_pos_Anim();
						//右手
						move_hand((*DrawPts)->tracker_num.size() > 0, m_inv, true, false);
						//左手
						this->LEFT_hand = (this->LEFTHAND.pos - this->LEFT_pos_Anim(0)).size() <= 0.1f && (!isReload() || !this->key_.have_mag);
						if (this->LEFT_hand) { this->LEFTHAND.pos = this->LEFT_pos_Anim(0); }
						move_hand((*DrawPts)->tracker_num.size() > 0, m_inv, true, true);
						//指
						this->anime_hand_nomal->per = 1.f;
						this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
						this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					}
				}
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
						easing_set(&this->ratio_r, ratio_t, 0.95f);
					}
					//足サウンド
					if (!this->get_jamping()) {
						if (this->key_.running) {
							if (abs(Head_bobbing(this->anime_run)) >= 0.8f && !this->audio.foot_.check()) {
								this->audio.foot_.play_3D(this->pos_gun, 15.f);
								this->audio.foot_.vol(255);
							}
						}
						else {
							if (!this->key_.squat) {
								if (abs(Head_bobbing(this->anime_walk)) >= 0.8f && !this->audio.foot_.check()) {
									this->audio.foot_.play_3D(this->pos_gun, 5.f);
									this->audio.foot_.vol(128);
								}
							}
							else {
								if (abs(Head_bobbing(this->anime_swalk)) >= 0.8f && !this->audio.foot_.check()) {
									this->audio.foot_.play_3D(this->pos_gun, 1.5f);
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
					this->obj_body.SetMatrix(MATRIX_ref::Mtrans(this->pos_tt - this->HMD.pos_rep));
					this->obj_body.SetFrameLocalMatrix(this->frame_s.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(this->frame_s.bodyg_f.second));
					this->obj_body.SetFrameLocalMatrix(this->frame_s.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(this->frame_s.bodyb_f.second));
					this->obj_body.SetFrameLocalMatrix(this->frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(this->frame_s.body_f.second));
					this->flag_calc_lag = true;
					//頭部
					{
						this->obj_body.SetFrameLocalMatrix(this->frame_s.head_f.first, this->HMD.mat*m_inv.Inverse()*MATRIX_ref::Mtrans(this->frame_s.head_f.second));
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
							ani_run = this->ratio_r;
						}
						else {
							if (!this->key_.squat) {
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
						this->LEFTHAND.pos = this->LEFT_pos_Anim(0);
						this->LEFTHAND.mat = this->mat_gun;
						this->LEFT_hand = ((this->LEFTHAND.pos - this->LEFT_pos_gun(0)).size() <= 0.1f);
						move_hand(false, m_inv, false, true);
						this->addpos_gun = this->add_vec;
						//指
						this->anime_hand_nomal->per = 1.f;
						this->anime_hand_trigger_pull->per = this->gunanime_trigger->per;
						this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
					}
				}
				else {
					//銃器
					{
						this->pos_gun += this->addpos_gun;
						this->addpos_gun.yadd(M_GR / std::powf(FPS, 2.f));
						auto down_y = VECTOR_ref::vget(0, 0.05f, 0);
						auto pp = (*MAPPTs)->map_col_line(this->pos_gun + VECTOR_ref::vget(0, 1.f, 0), this->pos_gun - down_y);
						if (pp.HitFlag) {
							set_gun_pos(VECTOR_ref(pp.HitPosition) + down_y, this->mat_gun * MATRIX_ref::RotVec2(this->mat_gun.xvec(), pp.Normal));
							easing_set(&this->addpos_gun, VECTOR_ref::vget(0, 0, 0), 0.8f);
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
		//メニュー限定
		//audio(menuのみ)
		const auto& get_audio(void) const noexcept { return audio; }
		//トリガーアニメーション取り出し(メニューのみ)
		const auto get_gunanime_trigger(void) const noexcept { return gunanime_trigger; }
	public:
		//銃座標指定
		void set_gun_pos(const VECTOR_ref& vec_t, const MATRIX_ref& mat_t) noexcept {
			this->pos_gun = vec_t;
			this->mat_gun = mat_t;
		}
		//
		const MATRIX_ref& Mag_Mat(void) noexcept {
			return this->mat_mag* MATRIX_ref::Mtrans(this->pos_mag);
		}
		const MATRIX_ref& Gun_Mat(void) noexcept {
			return this->mat_gun * MATRIX_ref::Mtrans(this->pos_gun);
		}
		//刺さっている時のマガジン座標
		void set_mag_pos(void) noexcept {
			this->mat_mag = this->mat_gun;
			this->pos_mag = this->base.mag2f_pos();
		}
		/*銃pos決定*/
		void Set_gun(void) noexcept {
			this->base.obj.SetMatrix(this->Gun_Mat());
			{
				this->underhandguard.Setpos_parts(this->mat_gun);
				this->uperhandguard.Setpos_parts(this->mat_gun);
				this->grip.Setpos_parts(this->mat_gun);
				this->mazzule.Setpos_parts(this->mat_gun);

				this->dustcover.Setpos_parts(this->mat_gun);
				this->stock.Setpos_parts(this->mat_gun);
				this->foregrip.Setpos_parts(this->mat_gun);
				this->lam.Setpos_parts(this->mat_gun);
				this->mount_base.Setpos_parts(this->mat_gun);
				this->mount_.Setpos_parts(this->mat_gun);
				for (auto&s : this->sight_) {
					s.Setpos_parts(this->mat_gun);
				}
			}
		}
		//刺さっているマガジンのMV1
		MV1* mag_obj(void) {
			if (this->gun_stat_now->hav_mags()) {
				return &gun_stat_now->get_magazine_in().front().obj_mag_;
			}
			return nullptr;
		}
		//マガジンの座標確定
		void Set_Magazine_Mat(void) noexcept {
			if (this->gun_stat_now->hav_mags()) {
				auto mat_t = Mag_Mat();
				this->mag_obj()->SetMatrix(mat_t);
				auto& mag_t = gun_stat_now->get_magazine_in().front();
				//todo
				mag_t.magazine_ammo_f[0] = this->magazine.magazine_ammo_f[0];
				mag_t.magazine_ammo_f[1] = this->magazine.magazine_ammo_f[1];
				mag_t.LEFT_mag_frame = this->magazine.LEFT_mag_frame[0];
				mag_t.magazine_f[0] = this->magazine.getmagazine_f(0);
				mag_t.magazine_f[1] = this->magazine.getmagazine_f(1);
				//
				mag_t.obj_ammo[0].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->mag_obj()->frame(mag_t.magazine_ammo_f[0].first) - mat_t.pos()));
				mag_t.obj_ammo[1].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->mag_obj()->frame(mag_t.magazine_ammo_f[1].first) - mat_t.pos()));
			}
		}
		//パーツアタッチ
		void Attach_parts(GUNPARTs* partsptr, size_t type_t, g_parts* parents, size_t side, size_t sight_mount = 0) noexcept {
			if (partsptr != nullptr) {
				switch (type_t) {
				case EnumGunParts::PARTS_NONE:
				{
					break;
				}
				case EnumGunParts::PARTS_NUM:
				{
					break;
				}
				case EnumGunParts::PARTS_BASE:
				{
					this->base.attach_(this->per_all, *partsptr, type_t);
					break;
				}
				case EnumGunParts::PARTS_MAGAZINE:
				{
					this->get_parts(type_t)->attach_(this->per_all, *partsptr, type_t);
					break;
				}
				case EnumGunParts::PARTS_SIGHT:
				{
					if (parents != nullptr) {
						if (sight_mount > 0) {
							if (this->sight_.size() < sight_mount) {
								this->sight_.resize(sight_mount);
							}
							this->sight_[sight_mount-1].attach_(this->per_all, *partsptr, type_t, parents, side);
						}
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
		void Detach_parts(size_t type_t, int sight_mount = 0) noexcept {
			switch (type_t) {
			case EnumGunParts::PARTS_NONE:
			{
				break;
			}
			case EnumGunParts::PARTS_NUM:
			{
				break;
			}
			case EnumGunParts::PARTS_BASE:
			{
				this->Detach_child(&this->base, type_t);
				break;
			}
			case EnumGunParts::PARTS_SIGHT:
			{
				if (this->sight_.size() > sight_mount) { this->Detach_child(&this->sight_[sight_mount], type_t, sight_mount); }
				break;
			}
			default:
				this->Detach_child(this->get_parts(type_t), type_t);
				break;
			}
		}
		//薬莢の生成
		void create_cart(void) noexcept {
			//弾
			this->bullet[this->use_bullet].Put(&this->base.thisparts->ammo[0], this->get_maz(), MATRIX_ref::Axis1(this->mat_gun.xvec()*-1.f, this->mat_gun.yvec()*-1.f, this->mat_gun.zvec()*-1.f));
			//薬莢待機
			this->cart[this->use_bullet].Put_first(this->base.thisparts->ammo[0].get_model(), this->base.cate_pos(), this->mat_gun);
			//
			++this->use_bullet %= this->bullet.size();//次のIDへ
		}
		//薬莢の演算
		void calc_cart(const float cart_rate_t) noexcept {
			//薬莢待機
			auto zpos_buf = this->base.obj.GetFrameLocalMatrix(this->base.slide_f.first).pos().z();
			for (auto& a : this->cart) {
				//待機中の移動
				a.Put_second(this->base.cate_pos(), this->mat_gun);
				//if (((this->zpos_cart - zpos_buf)*1000.f) >= -0.1f) {
					//解放
					a.Put_stay(this->base.cate_vec()*2.5f / FPS, cart_rate_t);
				//}
			}
			//clsDx();
			//printfDx("%0.5f", ((this->zpos_cart - zpos_buf)*1000.f));
			this->zpos_cart = zpos_buf;
		}
		//銃発砲エフェクトのセット
		void calc_shot_effect(void) noexcept {
			switch (this->mazzule.get_mazzuletype())
			{
			case 0:
				this->effcs[ef_fire].set(this->mazzule.mazzule_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//ノーマル
				break;
			case 1:
				this->effcs[ef_fire2].set(this->mazzule.mazzule_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//サプ
				break;
			default:
				this->effcs[ef_fire].set(this->base.mazzule_pos(), this->mat_gun.zvec()*-1.f, 0.0025f / 0.1f);//何もついてない
				break;
			}
		}
		//薬莢の処理
		void update_cart(void) noexcept {
			for (auto& a : this->cart) {
				if (a.Get_Flag()) {
					a.UpDate_pos();
					a.UpDate_fall((*MAPPTs)->map_col_line(a.Get_pos() + VECTOR_ref::vget(0, 1.f, 0), a.Get_pos() - VECTOR_ref::vget(0, 0.008f, 0)), *this);
				}
			}
		}
		//UI描画用用意
		void Set_Draw_bullet(void) noexcept {
			for (auto& a : this->bullet) {
				a.Set_Draw();
			}
		}
		//銃セレクターのアニメセット
		void Set_select_anime(void) noexcept {
			this->base.thisparts->Set_gun_select(this->gunanime_sel, this->gun_stat_now->selecter);
		}
		//銃発砲アニメのセット
		void Set_shot_anime(const float&rate, const bool isNotLast) noexcept {
			if (isNotLast) {
				this->gunanime_shot->per = 1.f;
				if (this->gunf) {
					this->gunanime_shot->update(true, rate*2.f*this->gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
					if (this->gunanime_shot->time == 0.f) {
						this->gunf = false;
					}
				}
			}
			else {
				this->gunanime_shot_last->per = 1.f;
				this->gunanime_shot_last->update(false, rate*2.f*this->gunanime_shot_last->alltime / (FRAME_RATE / (600.f / 60.f)));
				if (this->gunanime_shot_last->time == 0.f) {
					this->gunf = false;
				}
			}
		}
	public:
		//エフェクトの更新
		void update_effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs[0];
				if (index != ef_smoke) {
					t.put((*DrawPts)->get_effHandle((int32_t)(index)));
				}
			}
			for (auto& t : this->effcs_gndhit) {
				t.put((*DrawPts)->get_effHandle(ef_gndsmoke));
			}
		}
	public:
		/*使うスマポ*/
		void Get_ptr(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t,
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts_t,
			//std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs_t,
			std::unique_ptr<DeBuG, std::default_delete<DeBuG>>* DebugPTs_t
		) noexcept {
			DrawPts = DrawPts_t;
			//OPTPTs = OPTPTs_t;
			MAPPTs = MAPPTs_t;
			DebugPTs = DebugPTs_t;
		}
		/*キャラ設定*/
		void Set_(std::vector<GUNPARTs>& gun_data, size_t itr, MV1& body_, MV1& bodylag_, MV1& col_) noexcept {
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
				col_.DuplicateonAnime(&this->col, &this->obj_body);
				this->colframe_.get_frame(this->col);
				for (int i = 0; i < this->col.mesh_num(); ++i) {
					auto pp = this->col.SetupCollInfo(8, 8, 8, -1, i);
 					pp = 0;
				}
			}
			//変数
			{
				this->gunf = false;
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
				this->ani_timeline.resize(12);
				this->ani_lefthand.resize(3);
				this->view_ings = 0;
				this->reload_ings = 0;
				//
				this->HP_parts.resize(this->col.mesh_num());
				for (auto& h : this->HP_parts) {
					h = 1;
				}
			}
			//gun
			{
				//base
				Attach_parts(&gun_data[itr], EnumGunParts::PARTS_BASE, nullptr, 0);
				//baseanime
				this->gunanime_first = &this->base.obj.get_anime(0);
				this->gunanime_shot = &this->base.obj.get_anime(1);
				this->gunanime_shot_last = &this->base.obj.get_anime(2);
				this->gunanime_trigger = &this->base.obj.get_anime(3);
				this->gunanime_magcatch = &this->base.obj.get_anime(4);
				this->gunanime_sel.resize(this->base.thisparts->select.size());
				this->gunanime_sel[0] = &this->base.obj.get_anime(5);
				this->gunanime_sel[1] = &this->base.obj.get_anime(6);
				//gunstat
				this->gun_stat.resize(gun_data.size());
				for (auto& i : this->gun_stat) { i.Set(); }
				this->gun_stat_now = &gun_stat[this->base.thisparts->id_t];
			}
			for (auto& a : this->cart) { a.Set(); }				//薬莢
			for (auto& a : this->bullet) { a.Set(MAPPTs); }		//bullet
			this->audio.Duplicate(this->base.thisparts->audio);	//audio
		}
		/*キャラスポーン*/
		void Spawn(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) noexcept {
			for (auto&s : this->sight_) {
				if (s.get_attaching()) {
					s.sight_vec = s.get_attach_frame() + s.get_sight_frame();
					search_sight_base(&s.sight_vec, s);
				}
			}

			this->xrad_p = 0;

			this->spawn_pos = pos_;
			this->spawn_mat = mat_H;

			this->pos_tt = this->spawn_pos;
			this->HMD.mat = this->spawn_mat;
			this->mat_notlean = this->spawn_mat;

			this->key_.ReSet_();

			this->add_ypos = 0.f;
			this->add_vec_buf.clear();
			this->distance_to_cam = -1.f;
			this->flag_calc_body = true;
			this->flag_calc_lag = true;

			this->HP = this->HP_full;
			this->HP_parts.resize(this->col.mesh_num());
			for (auto& h : this->HP_parts) {
				h = 1;
			}

			this->scores.reset(false);

			this->gun_stat_now->Set();
			for (int i = 0; i < 6; ++i) {
				this->gun_stat_now->magazine_insert(this->magazine.thisparts);			//マガジン+1(装填あり)
			}

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度

			this->gunf = false;
			this->gunanime_shot->reset();
			this->gunanime_shot_last->reset();
			this->gunanime_trigger->reset();
			this->gunanime_magcatch->reset();
			this->gunanime_first->reset();
			this->gunanime_sel[0]->reset();
			this->gunanime_sel[1]->reset();

			prev_ani_lefthand = &this->ani_lefthand[0];
			next_ani_lefthand = &this->ani_lefthand[0];
			this->nearhit = false;
			//AIの選択をリセット
			{
				int now = (*MAPPTs)->get_next_waypoint(this->cpu_do.wayp_pre, this->get_pos());
				this->cpu_do.spawn((now != -1) ? now : 0);
			}
		}
		/*最初*/
		void Start(void) noexcept {
			this->HMD.Set(DrawPts);
			this->WAIST.Set(DrawPts);
			this->LEFTREG.Set(DrawPts);
			this->RIGHTREG.Set(DrawPts);
			this->distance_to_cam = -1.f;
			this->flag_calc_body = false;
			this->flag_start_loop = true;
			this->flag_calc_lag = true;
			this->ratio_r = 0.f;
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
				this->frame_s.copy_frame(this->obj_body, this->colframe_, &this->col);
				for (int i = 0; i < this->col.mesh_num(); ++i) {
					this->col.RefreshCollInfo(-1, i);
				}
				return true;
			}
			return false;
		}
		/*更新*/
		void UpDate_( HIT_PASSIVE& hit_obj_p, HIT_BLOOD_PASSIVE& hit_b_obj_p, const bool playing, const float fov_per, std::vector <Meds>& meds_data, std::vector<Chara>&chara, Chara&mine, const bool use_vr) noexcept {
			/*
			全体　0.4ms〜1.4ms
			*/
			this->nearhit = false;
			{
				easing_set(&this->HP_r, float(this->HP), 0.95f);
				easing_set(&this->got_damage_f, 0.f, 0.95f);
				//
				{
					for (auto& d : this->got_damage_) {
						easing_set(&d.alpfa, 0.f, 0.975f);
					}
					while (true) {
						bool none = true;
						for (auto& d : this->got_damage_) {
							if (std::clamp(int(255.f*(1.f - powf(1.f - d.alpfa, 5.f))), 0, 255) < 5) {
								auto i = (&d - &this->got_damage_[0]);
								this->got_damage_.erase(this->got_damage_.begin() + i);
								none = false;
							}
						}
						if (none) {
							break;
						}
					}
				}
				if (!this->get_alive()) {
					this->HMD.pos.clear();
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
						this->Spawn(this->spawn_pos, this->spawn_mat);
						this->gunanime_shot->per = 1.f;
					}
				}
				this->scores.UpDate();
			}
			//座標取得
			if (this == &mine) {
				if ((*DrawPts)->tracker_num.size() > 0) {
					this->WAIST.UpDate_pos((*DrawPts)->tracker_num[0], flag_start_loop);
					this->WAIST.pos = this->WAIST.pos - this->WAIST.pos_rep;
				}
			}
//p1〜p2 0.00ms
			//プレイヤー操作
			this->operation(!playing, chara, fov_per, (this == &mine) ? 0 : 1, use_vr);
//p2〜p2-1 〜0.11ms
			//壁その他の判定
			this->wall_col(mine, use_vr);
//p2-1〜p3 〜0.04ms
			//obj演算
			this->Set_body(use_vr, mine);
//p3〜p4 0.15〜0.25ms
			//lag演算
			//if (!this->get_alive()) {
				this->frame_s.copy_frame(this->obj_body, this->lagframe_, &this->obj_lag);
				this->obj_lag.work_anime();
			//}
//p4〜p4-2 0.03ms
			//col演算
			if (this->get_alive()) {
				this->col.work_anime();
			}
//p4-2〜p4-3 0.01ms
			//
			if (!(use_vr && (this == &mine))) {
				//視点取得
				set_HMDpos();
				//銃器
				if (this->get_alive()) {
					Set_gun();
					if (isReload()) {
						auto ans1_ = this->LEFT_pos_Anim(0);
						auto ans2_ = this->LEFT_pos_Anim(1);
						auto ans3_ = this->LEFT_pos_Anim(2);
						this->mat_mag = this->mat_gun;
						this->pos_mag = ans1_;
						Set_Magazine_Mat();
						this->mat_mag = MATRIX_ref::Axis1_YZ((ans3_ - ans1_).Norm(), (ans2_ - ans1_).Norm()*-1.f);
						if (this->gun_stat_now->hav_mags()) {
							this->pos_mag += (this->pos_mag - this->gun_stat_now->get_magazine_in().front().LEFT_mag_pos());
						}
					}
				}
			}
//p4-3〜p5 0.01ms
			//射撃関連
			{
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
				easing_set(&this->blur_vec_res, VECTOR_ref::vget(0, 0, 1.f), 0.95f);
				//複座
				easing_set(&this->recoil_vec, this->recoil_vec_res, 0.6f);
				easing_set(&this->recoil_vec_res, VECTOR_ref::vget(0, 0, 1.f), 0.95f);
				//リコイルアニメーション
				if (this->gunf) {
					Set_shot_anime(1.f, this->gun_stat_now->not_chamber_EMPTY());
					if (this->gun_stat_now->not_chamber_EMPTY()) {
						this->gunanime_shot_last->reset();
					}
					else {
						this->gunanime_shot->reset();
					}
				}
				//マガジン排出(通常)
				if (this->key_.reload && (!this->reloadf && this->gun_stat_now->hav_mags())) {
					this->reloadf = true;
					magrelease_t(1);
				}
				//medkit生成
				if (this->key_.drop_) {
					medrelease(&meds_data[0]);
				}
				//眺める
				if (!this->view_ing && this->key_.view_gun) {
					this->view_ing = true;
				}
				//マガジン整頓
				if (!this->sort_ing && this->key_.sort_magazine && this->gun_stat_now->get_magazine_in().size() >= 2) {
					this->sort_ing = true;
					if (!this->sort_f) {
						this->sort_f = true;
						this->audio.sort_magazine.play_3D(this->pos_gun, 15.f);
						this->gun_stat_now->sort_magazine();
					}
					else {
						this->audio.load_.play_3D(this->pos_gun, 15.f);
						this->gun_stat_now->load_magazine();
						//マガジン輩出(空)
						if (this->gun_stat_now->get_magazine_in().back().m_cnt == 0) {
							magrelease_t(2);
						}
					}
				}
				//マガジン挿入
				if (isReload()) {
					if ((*DrawPts)->use_vr && this->reload_cnt < this->base.thisparts->reload_time) {
						this->key_.have_mag = false;
					}
					if (this->key_.have_mag) {
						if (this->gun_stat_now->hav_mags()) {
							auto mag0 = this->gun_stat_now->get_magazine_in().front().getmagazine_f(0).first;
							auto mag1 = this->gun_stat_now->get_magazine_in().front().getmagazine_f(1).first;
							if (
								((*DrawPts)->use_vr) ?
								((this->mag_obj()->frame(mag1) - this->base.magf_pos()).size() <= 0.1f) :
								(this->reload_cnt > this->base.thisparts->reload_time)
								) {
								this->gunanime_shot->per = 1.f;
								this->gunanime_shot_last->reset();
								if (!this->gun_stat_now->not_chamber_EMPTY()) {
									this->gunanime_first->reset();
									this->gunanime_first->per = 1.f;
								}
								this->gun_stat_now->magazine_slide();//チャンバーに装填

								this->audio.magazine_Set.play_3D(this->pos_gun, 15.f);
								this->reloadf = false;
							}
							if ((*DrawPts)->use_vr) {
								this->pos_mag = this->LEFTHAND.pos;
								this->mat_mag = this->mag_obj()->GetFrameLocalMatrix(mag1) * this->mag_obj()->GetFrameLocalMatrix(mag0) *
									(this->LEFTHAND.mat* MATRIX_ref::RotVec2(this->LEFTHAND.mat.yvec(), this->base.mag2f_pos() - this->LEFTHAND.pos));
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
					++this->gun_stat_now->selecter %= this->base.thisparts->select.size();
				}
				//射撃
				if (!this->gunf && this->gun_stat_now->not_chamber_EMPTY()) {
					if (this->base.thisparts->Select_Chose(EnumSELECTER::SELECT_FULL) == int(this->gun_stat_now->selecter)) {
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
					auto ppud = (this->LEFT_hand ? 3.f : 1.f) * (this->key_.squat ? 1.75f : 1.0f);							//持ち手を持つとココが相殺される

					easing_set(&this->blur_vec_res,
						MATRIX_ref::Vtrans(this->blur_vec_res,
							MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn*100.f) + GetRand((int32_t)((xup - xdn)*100.f))) / (100.f*ppud)))*
							MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn*100.f) + GetRand((int32_t)((yup - ydn)*100.f))) / (100.f*ppud))))
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
					if (!this->gunf && this->gun_stat_now->not_chamber_EMPTY() && this->gunanime_first->per == 0.f && !this->view_ing) {
						this->gunf = true;
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
				//薬莢
				this->calc_cart(1.f);
				//
				this->Set_Magazine_Mat();
				//弾の処理
				for (auto& a : this->bullet) {
					a.UpDate(this, &chara, hit_obj_p, hit_b_obj_p);
				}
				//薬莢の処理
				update_cart();
				//エフェクトの処理
				update_effect();
				//銃アニメ更新
				this->base.obj.work_anime();
			}
//p5〜p5-2 0.08ms
			//アイテム拾う
			(*MAPPTs)->Get_item(this->get_pos_LEFTHAND(), this->get_pos_LEFTHAND() - this->get_mat_LEFTHAND().zvec()*2.6f, *this, *MAPPTs);
//p5-2〜p5-3 0.01ms
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
//p5-3〜p6 0.04ms
			//
			if (this == &mine) {
				//レティクル
				for (auto&s : this->sight_) {
					s.Set_reticle();
				}
				//ライト
				this->lam.Set_LightHandle(this->base.obj.GetMatrix().zvec()*-1.f);
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
//p6〜p7 0.01ms
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
				if ((*MAPPTs)->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 1.8f, 0)).HitFlag &&
					(*MAPPTs)->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.f, 0)).HitFlag) {
					this->flag_canlook_player = false;
					this->flag_calc_body = true;
					this->distance_to_cam = -1.f;
					return;
				}
			}
		}
		/*カメラ指定*/
		void Set_cam(cam_info& camera_main, std::vector<Chara>&chara, const float fov_, const bool use_vr) noexcept {
			if (this->get_alive()) {
				auto mat_T = get_res_blur(0.7f) * this->HMD.mat;//リコイル

				auto ppsh = MATRIX_ref::Vtrans(VECTOR_ref::vget(-0.035f, 0, 0), mat_T);
				if (use_vr) {
					mat_T = this->HMD.mat;
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
				easing_set(&camera_main.camvec, chara[this->scores.death_id].get_pos(), 0.9f);
				auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
				easing_set(&camera_main.campos, this->get_pos() + VECTOR_ref::vget(-5.f*sin(rad), 2.f, -5.f*cos(rad)), 0.9f);
				camera_main.camup = VECTOR_ref::vget(0, 1.f, 0);
				(*MAPPTs)->map_col_nearest(camera_main.camvec, &camera_main.campos);
				easing_set(&camera_main.fov, fov_, 0.9f);
			}
		}
		/*キャラ+銃描画*/
		void Draw_chara(int drawlevel = 1) noexcept {
			if (this->flag_canlook_player) {
				switch (drawlevel){
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
						//this->col.DrawModel();
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
			this->base.obj.DrawModel();
			if ((!isReload() || this->have_magazine) && this->gun_stat_now->hav_mags()) {
				this->gun_stat_now->get_magazine_in().front().Draw();
			}
			//
			if (drawlevel != 0) {
				if (this->distance_to_cam <= 5.f) {
					for (auto& a : this->cart) {
						a.Draw();
					}
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
					for (auto&s : sight_) {
						s.Draw();
					}
				}
			}
			//
		}
		/*弾道描画*/
		void Draw_ammo(void) noexcept {
			for (auto& a : this->bullet) {
				a.Draw();
			}
		}
		/*レーザー、ライト描画*/
		void Draw_LAM_Effect(std::vector<Chara>&chara, GraphHandle&light_pic) noexcept {
			if (this->lam.get_attaching()) {
				auto StartPos = this->lam.source_frame_pos();
				switch (this->lam.thisparts->select_lam[0]){
				case EnumSELECT_LAM::SELECT_LASER:
				{
					auto EndPos = StartPos - this->base.obj.GetMatrix().zvec()*100.f;
					(*MAPPTs)->map_col_nearest(StartPos, &EndPos);
					for (auto& tgt : chara) {
						if (&tgt == this || !tgt.get_alive()) {
							continue;
						}
						if (tgt.set_ref_col(StartPos, EndPos)) {
							for (int i = 0; i < this->col.mesh_num(); ++i) {
								auto q = tgt.col.CollCheck_Line(StartPos, EndPos, -1, i);
								if (q.HitFlag) {
									EndPos = q.HitPosition;
								}
							}
						}
					}
					SetUseLighting(FALSE);
					SetFogEnable(FALSE);

					DXDraw::Capsule3D(StartPos, EndPos, 0.001f, GetColor(255, 0, 0), GetColor(255, 255, 255));
					DrawSphere3D(EndPos.get(), std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.05f), 6, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
					break;
				}
				case EnumSELECT_LAM::SELECT_LIGHT:
				{
					DrawBillboard3D(StartPos.get(), 0.5f, 0.5f, 0.1f, 0.f, light_pic.get(), TRUE);
					break;
				}
				default:
					break;
				}
			}
		}
		/*UI描画*/
		/*HPバーを表示する場所*/
		const auto Set_HP_UI(const VECTOR_ref& cam_pos) noexcept {
			if ((this->HP - int(this->HP_r)) <= -2) {
				auto head = this->get_head_pos();
				return VECTOR_ref(ConvWorldPosToScreenPos((head + VECTOR_ref::vget(0, 0.3f + 2.7f*std::max((head - cam_pos).size(), 1.f) / 100.f, 0)).get()));
			}
			else {
				return VECTOR_ref::vget(0.f, 0.f, -1.f);
			}
		}
		/* UI向けにヒット部分を表示*/
		void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
			for (auto& a : this->bullet) {
				if (255.f*a.hit_alpha >= 10) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f*a.hit_alpha));
					hit_Graph.DrawRotaGraph(a.hit_window_x, a.hit_window_y, a.hit_alpha*0.5f, 0.f, true);//(ismine ? 1.f : 0.5f)
				}
			}
		}
		/*おわり*/
		void Dispose(void) noexcept {
			//パーツリセット
			Detach_parts(EnumGunParts::PARTS_BASE);
			Detach_parts(EnumGunParts::PARTS_MAGAZINE);
			Detach_parts(EnumGunParts::PARTS_LAM);
			Detach_parts(EnumGunParts::PARTS_MAZZULE);
			Detach_parts(EnumGunParts::PARTS_GRIP);
			Detach_parts(EnumGunParts::PARTS_UPER_HGUARD);
			Detach_parts(EnumGunParts::PARTS_UNDER_HGUARD);
			Detach_parts(EnumGunParts::PARTS_DUSTCOVER);
			Detach_parts(EnumGunParts::PARTS_STOCK);
			Detach_parts(EnumGunParts::PARTS_FOREGRIP);
			Detach_parts(EnumGunParts::PARTS_MOUNT_BASE);
			Detach_parts(EnumGunParts::PARTS_MOUNT);

			Detach_parts(EnumGunParts::PARTS_SIGHT);
			Detach_parts(EnumGunParts::PARTS_SIGHT);
			Detach_parts(EnumGunParts::PARTS_SIGHT);
			//
			this->obj_body.Dispose();
			this->col.Dispose();
			//
			this->audio.Dispose();
			for (auto& a : this->cart) { a.Dispose(); }
			for (auto& t : this->effcs) { t.handle.Dispose(); }
			for (auto& t : this->effcs_gndhit) { t.handle.Dispose(); }
			for (auto& i : this->gun_stat) { i.Set(); }
			this->gun_stat.clear();
		}
	};
};