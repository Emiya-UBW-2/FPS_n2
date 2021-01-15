#pragma once
class main_c : Mainclass {
	//設定
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	float fov_pc = 45.f;
	//カメラ
	DXDraw::cam_info camera_main, camera_sub, camera_TPS;
	//描画スクリーン
	GraphHandle outScreen2;
	GraphHandle UI_Screen,UI_Screen2;			
	//操作
	switchs TPS;
	float xr_cam = 0.f;
	float yr_cam = 0.f;
	int sel_cam = 0;
	//データ
	MV1 body_obj,body_col;			//身体モデル
	std::vector<Guns>  gun_data;	//GUNデータ
	std::vector<Mags> mag_data;		//GUNデータ
	std::vector<Meds> meds_data;	//GUNデータ
	//オブジェ
	std::vector<Chara> chara;		//キャラ
	std::vector<Items> item;		//アイテム
	//仮
	bool oldv_1_1 = false;
	bool oldv_1_2 = false;
	bool oldv_2_1 = false;
	bool oldv_2_2 = false;
	bool oldv_3_1 = false;
	bool oldv_3_2 = false;
	//
	bool ending = true;
public:
	main_c() {
		//設定読み込み
		{
			SetOutApplicationLogValidFlag(FALSE);
			int mdata = FileRead_open("data/setting.txt", FALSE);
			dof_e = getparams::_bool(mdata);
			bloom_e = getparams::_bool(mdata);
			shadow_e = getparams::_bool(mdata);
			useVR_e = getparams::_bool(mdata);
			fov_pc = getparams::_float(mdata);
			FileRead_close(mdata);
			SetOutApplicationLogValidFlag(TRUE);
		}
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, useVR_e, shadow_e);							//汎用
		auto UIparts = std::make_unique<UI>();																		//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);														//デバッグ
		auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->disp_x, Drawparts->disp_y);//ホストパスエフェクト(VR、フルスクリーン共用)
		UI_Screen = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y, true);									//VR、フルスクリーン共用UI
		auto Hostpass2parts = std::make_unique<HostPassEffect>(dof_e, bloom_e, deskx, desky);						//ホストパスエフェクト(フルスクリーン向け)
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS用描画スクリーン
		UI_Screen2 = GraphHandle::Make(deskx, desky, true);															//フルスクリーン向けUI
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		auto minimapparts = std::make_unique<Minimapclass>();														//MINIMAP
		auto ruleparts = std::make_unique<rule>();																	//ルール
		//model
		MV1::Load("data/model/body/model.mv1", &this->body_obj, true);	//身体
		MV1::Load("data/model/body/col.mv1", &this->body_col, true);	//身体col
		//ミニマップ
		minimapparts->load();
		//auto font = FontHandle::Create(18);
		//GUNデータ
		{
			this->gun_data.resize(1);
			this->gun_data[0].mod.Ready("data/gun/", "GUN");
		}
		//MAGデータ
		{
			this->mag_data.resize(1);
			this->mag_data[0].mod.Ready("data/mag/", "GUN");
		}
		//MEDデータ
		{
			this->meds_data.resize(1);
			this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
		}
		UIparts->load_window("アイテムオブジェクト");	//ロード画面1
		//GUNデータ2
		for (auto& g : this->gun_data) {
			g.id = &g - &this->gun_data[0];
			g.set_data(this->mag_data);
		}
		//MAGデータ2
		for (auto& g : this->mag_data) {
			g.id = &g - &this->mag_data[0];
			g.set_data();
		}
		//MEDデータ2
		for (auto& g : this->meds_data) {
			g.id = &g - &this->meds_data[0];
			g.set_data();
		}
		UIparts->load_window("アイテムデータ");	//ロード画面2
		do {
			//マップ読み込み
			mapparts->Ready_map("data/map_new2");			//mapparts->Ready_map("data/new");
			UIparts->load_window("マップ");
			mapparts->Set_map("data/maps/set.txt", this->item,
				//this->gun_data,
				this->mag_data,
				this->meds_data
			);
			//キャラ設定
			size_t sel_g = 0;
			chara.resize(mapparts->get_spawn_point().size());
			auto& mine = chara[0];
			{
				//自機セット
				mine.set(this->gun_data, sel_g, this->body_obj, this->body_col);
				mine.spawn(mapparts->get_spawn_point()[0], MATRIX_ref::RotY(
					atan2f(mapparts->get_spawn_point()[0].x(), mapparts->get_spawn_point()[0].z())
				));
				//その他
				for (int i = 1; i < mapparts->get_spawn_point().size(); i++) {
					chara[i].set(this->gun_data, sel_g, this->body_obj, this->body_col);
					chara[i].spawn(mapparts->get_spawn_point()[i], MATRIX_ref::RotY(
						atan2f(mapparts->get_spawn_point()[i].x(), mapparts->get_spawn_point()[i].z())
					));
				}
			}
			//ライティング
			Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {
				for (int i = 0; i < 2; i++) {
					mapparts->map_get().DrawMesh(i);
				}
			});
			//描画するものを指定する(仮)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					mapparts->map_get().DrawMesh(0);
					mapparts->map_get().DrawMesh(1);
					mapparts->map_get().DrawMesh(2);
					SetFogEnable(FALSE);
					mapparts->map_get().DrawMesh(3);
					SetFogEnable(TRUE);
					/*
					for (int i = 1; i < mapparts->map_get().mesh_num(); i++) {
						mapparts->map_get().DrawMesh(i);
					}
					//*/

					for (auto& c : this->chara) {
						c.Draw_chara();
						{
							VECTOR_ref startpos = c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first);
							VECTOR_ref endpos = startpos - c.obj_gun.GetMatrix().zvec()*100.f;
							mapparts->map_col_nearest(startpos, &endpos);

							for (auto& tgt : this->chara) {
								if (&tgt == &c) {
									continue;
								}
								for (int i = 0; i < 3; i++) {
									auto q = tgt.col.CollCheck_Line(startpos, endpos, -1, i);
									if (q.HitFlag) {
										endpos = q.HitPosition;
									}
								}
							}
							SetUseLighting(FALSE);
							SetFogEnable(FALSE);

							DXDraw::Capsule3D(startpos, endpos, 0.001f, GetColor(255, 0, 0), GetColor(255, 255, 255));
							DrawSphere3D(endpos.get(), std::clamp(powf((endpos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.05f), 6, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

							SetUseLighting(TRUE);
							SetFogEnable(TRUE);
						}
					}
					for (auto& g : this->item) {
						g.Draw_item(this->chara[0]);
					}
					//銃弾
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) {
						c.Draw_ammo();
					}
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			//開始
			{
				//環境
				mapparts->Start_map();
				//プレイヤー操作変数群
				this->TPS.ready(false);
				oldv_3_2 = true;
				oldv_2_2 = true;
				for (auto& c : chara) {
					c.start();
					c.reset_waypoint(mapparts);
				}
				SetMousePoint(deskx / 2, desky / 2);
				//1P
				camera_main.fov = deg2rad(Drawparts->use_vr ? 90 : fov_pc);	//
				camera_main.near_ = 0.1f;
				camera_main.far_ = 100.f;
				//2P
				camera_sub.fov = deg2rad(fov_pc);	//
				camera_sub.near_ = 0.1f;
				camera_sub.far_ = 100.f;
				//TPS
				camera_TPS.campos = VGet(0, 1.8f, -10);
				camera_TPS.fov = deg2rad(fov_pc);
				camera_TPS.near_ = 0.1f;
				camera_TPS.far_ = 100.f;

				ruleparts->set();
				//
				while (ProcessMessage() == 0) {
					const auto fps_ = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//座標取得
						if (Drawparts->tracker_num.size() > 0) {
							auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[0]];
							Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &mine.pos_WAIST, &mine.mat_WAIST);
							if (mine.start_c || (ptr_.turn && ptr_.now) != oldv_1_2) {
								mine.pos_WAIST_rep = VGet(mine.pos_WAIST.x(), 0.f, mine.pos_WAIST.z());
							}
							oldv_1_2 = ptr_.turn && ptr_.now;
							mine.pos_WAIST = mine.pos_WAIST - mine.pos_WAIST_rep;
						}
						{
							for (auto& c : chara) {
								easing_set(&c.HP_r, float(c.HP), 0.95f);
								if (c.HP == 0) {
									easing_set(&c.body.get_anime(0).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(1).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(2).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(3).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(5).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(6).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(7).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(8).per, 0.f, 0.9f);
									easing_set(&c.body.get_anime(9).per, 0.f, 0.9f);

									easing_set(&c.body.get_anime(4).per, 1.f, 0.9f);

									c.body.get_anime(4).time += 30.f / fps_;
									if (c.body.get_anime(4).time >= c.body.get_anime(4).alltime) {
										c.body.get_anime(4).time = c.body.get_anime(4).alltime;
									}
									if (c.death_timer == 0.f) {
										c.body.frame_reset(c.frame_s.bodyg_f.first);
										c.body.frame_reset(c.frame_s.bodyb_f.first);
										c.body.frame_reset(c.frame_s.body_f.first);
										c.body.frame_reset(c.frame_s.head_f.first);
										c.body.frame_reset(c.frame_s.RIGHTarm1_f.first);
										c.body.frame_reset(c.frame_s.RIGHTarm1_f.first);
										c.body.frame_reset(c.frame_s.RIGHTarm2_f.first);
										c.body.frame_reset(c.frame_s.RIGHTarm2_f.first);
										c.body.frame_reset(c.frame_s.RIGHThand_f.first);
										c.body.frame_reset(c.frame_s.LEFTarm1_f.first);
										c.body.frame_reset(c.frame_s.LEFTarm1_f.first);
										c.body.frame_reset(c.frame_s.LEFTarm2_f.first);
										c.body.frame_reset(c.frame_s.LEFTarm2_f.first);
										c.body.frame_reset(c.frame_s.LEFThand_f.first);

										//マガジン排出
										if (c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
											//音
											c.audio.mag_down.play_3D(c.pos_RIGHTHAND, 15.f);
											//弾数
											auto dnm = (c.gun_stat[c.gun_ptr->id].ammo_cnt >= 1) ? c.gun_stat[c.gun_ptr->id].ammo_cnt - 1 : 0;
											while (true) {
												//マガジン排出
												c.gun_stat[c.gun_ptr->id].mag_release();
												//マガジン排出
												bool tt = false;
												for (auto& g : this->item) {
													if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
														tt = true;
														g.Set_item(c.gun_ptr->magazine, c.pos_mag, c.mat_mag);
														g.add = (c.obj_gun.frame(c.gun_ptr->frame_s.magazine2_f.first) - c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
														g.magazine.cap = dnm;
														g.del_timer = 0.f;
														break;
													}
												}
												if (!tt) {
													this->item.resize(this->item.size() + 1);
													auto& g = this->item.back();
													g.id = this->item.size() - 1;
													g.Set_item(c.gun_ptr->magazine, c.pos_mag, c.mat_mag);
													g.add = VECTOR_ref(VGet(
														float(-10 + GetRand(10 * 2)) / 100.f,
														-1.f,
														float(-10 + GetRand(10 * 2)) / 100.f
													))*1.f / fps_;//排莢ベクトル
													g.magazine.cap = dnm;
													g.del_timer = 0.f;
												}
												//
												if (c.gun_stat[c.gun_ptr->id].mag_in.size() == 0) {
													break;
												}
												dnm = c.gun_stat[c.gun_ptr->id].mag_in.front();
											}
										}
									}
									c.death_timer += 1.f / fps_;
									if (c.death_timer >= 5.f) {
										c.death_timer = 0.f;
										c.body.get_anime(4).per = 0.f;
										c.body.get_anime(4).time = 0.f;
										c.spawn(c.spawn_pos, c.spawn_mat);
										c.reset_waypoint(mapparts);
									}

								}
								if (c.kill_f) {
									c.kill_time -= 1.f / fps_;
									if (c.kill_time <= 0.f) {
										c.kill_time = 0.f;
										c.kill_streak = 0;
										c.kill_f = false;
									}
								}
							}
						}
						//プレイヤー操作
						{
							//cpu
							for (auto& c : chara) {
								if (&c - &chara[0] >= (Drawparts->use_vr ? 1 : 1)) {
									c.operation_NPC(mapparts, chara, !ruleparts->getstart() || ruleparts->getend());
									c.operation_3();
								}
							}
							//chara
							if (Drawparts->use_vr) {
								mine.operation_VR(Drawparts, !ruleparts->getstart() || ruleparts->getend(), &oldv_1_1);
								mine.operation_3();
								//2P
								/*
								{
									auto& ct = chara[1];
									ct.operation(!ruleparts->getstart() || ruleparts->getend());
									ct.operation_3();
								}
								//*/
							}
							else {
								mine.operation(!ruleparts->getstart() || ruleparts->getend());
								mine.operation_3();
							}
						}
						//
						for (auto& c : chara) {
							//壁その他の判定
							{
								//VR用
								{
									VECTOR_ref pos_t2 = (c.pos + (VGet(c.pos_HMD_old.x(), 0.f, c.pos_HMD_old.z())) - c.rec_HMD);
									VECTOR_ref pos_t = (c.pos + (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
									//壁
									mapparts->map_col_wall(pos_t2, &pos_t);
									c.pos = pos_t - (VECTOR_ref(VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
								}
								//共通
								{
									VECTOR_ref pos_t2 = (c.pos + (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
									VECTOR_ref pos_t = pos_t2 + c.add_pos;
									//壁
									mapparts->map_col_wall(pos_t2, &pos_t);
									//落下
									{
										auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.8f, 0), pos_t);
										if (c.add_ypos <= 0.f && pp.HitFlag) {
											pos_t = pp.HitPosition;
											c.add_ypos = 0.f;
										}
										else {
											pos_t.yadd(c.add_ypos);
											c.add_ypos += M_GR / std::powf(fps_, 2.f);
											//復帰
											if (pos_t.y() <= -5.f) {
												pos_t = c.spawn_pos;
												if (!Drawparts->use_vr && (&c == &mine)) {
													c.xrad_p = 0;
												}
												c.spawn(pos_t, c.spawn_mat);
												c.reset_waypoint(mapparts);
											}
										}
									}
									//反映
									c.pos = pos_t - (VECTOR_ref(VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
									c.add_pos_buf2 = pos_t - pos_t2;
								}
							}
							//pos
							if (Drawparts->use_vr && (&c == &mine)) {
								{
									VECTOR_ref v_ = c.mat.zvec();
									if (Drawparts->tracker_num.size() > 0) {
										v_ = c.mat_WAIST.zvec();
									}
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * FRAME_RATE / fps_ / 10.f;
								}
								//身体,頭部,腰
								MATRIX_ref m_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								{
									c.body.SetMatrix(m_inv);
									if (Drawparts->tracker_num.size() > 0) {
										//腰
										c.body.SetFrameLocalMatrix(c.frame_s.bodyb_f.first, (c.mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(c.frame_s.bodyb_f.second));
										//頭部
										c.body.SetFrameLocalMatrix(c.frame_s.head_f.first, (MATRIX_ref::Axis1(c.mat.xvec()*-1.f, c.mat.yvec(), c.mat.zvec()*-1.f) *m_inv.Inverse()*(c.mat_WAIST*m_inv.Inverse()).Inverse())
											*MATRIX_ref::Mtrans(c.frame_s.head_f.second));
									}
									else {
										//頭部
										c.body.SetFrameLocalMatrix(c.frame_s.head_f.first, (MATRIX_ref::Axis1(c.mat.xvec()*-1.f, c.mat.yvec(), c.mat.zvec()*-1.f) *m_inv.Inverse())
											*MATRIX_ref::Mtrans(c.frame_s.head_f.second));
									}
									c.body.SetMatrix(m_inv *MATRIX_ref::Mtrans((c.pos + c.pos_HMD - c.rec_HMD) - (c.body.frame(c.frame_s.RIGHTeye_f.first) + (c.body.frame(c.frame_s.LEFTeye_f.first) - c.body.frame(c.frame_s.RIGHTeye_f.first))*0.5f)));
								}
								//足
								{
									//左
									if (Drawparts->tracker_num.size() > 1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[1]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &mine.pos_LEFTREG, &mine.mat_LEFTREG);
										c.mat_LEFTREG = MATRIX_ref::Axis1(c.mat_LEFTREG.xvec()*-1.f, c.mat_LEFTREG.yvec(), c.mat_LEFTREG.zvec()*-1.f);
										if ((mine.start_c || (ptr_.turn && ptr_.now) != oldv_2_1) && oldv_2_2) {
											mine.mat_LEFTREG_rep = mine.mat_LEFTREG;
											if (!mine.start_c) {
												//oldv_2_2 = false;
											}
										}
										oldv_2_1 = ptr_.turn && ptr_.now;
										mine.mat_LEFTREG =
											MATRIX_ref::RotY(deg2rad(90 + 60 - 10))*
											mine.mat_LEFTREG_rep.Inverse()*mine.mat_LEFTREG;
										mine.pos_LEFTREG = mine.pos_LEFTREG + (mine.pos - mine.rec_HMD);
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_LEFTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_s.LEFTreg_f.first) - c.body.frame(c.frame_s.LEFTfoot2_f.first)).size(), (c.body.frame(c.frame_s.LEFTfoot2_f.first) - c.body.frame(c.frame_s.LEFTfoot1_f.first)).size(), (c.body.frame(c.frame_s.LEFTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTfoot2_f.first) - c.body.frame(c.frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTfoot1_f.second));

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTreg_f.first) - c.body.frame(c.frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTreg_f.first, c.mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFTreg_f.second));
										}

										{
											auto pp = mapparts->map_col_line(mine.body.frame(mine.frame_s.LEFTreg2_f.first) + VGet(0, 1.8f, 0), mine.body.frame(mine.frame_s.LEFTreg2_f.first));
											if (pp.HitFlag) {
												mine.pos_LEFTREG = VECTOR_ref(pp.HitPosition) - (mine.body.frame(mine.frame_s.LEFTreg2_f.first) - mine.body.frame(mine.frame_s.LEFTreg_f.first));
											}
										}

										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_LEFTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_s.LEFTreg_f.first) - c.body.frame(c.frame_s.LEFTfoot2_f.first)).size(), (c.body.frame(c.frame_s.LEFTfoot2_f.first) - c.body.frame(c.frame_s.LEFTfoot1_f.first)).size(), (c.body.frame(c.frame_s.LEFTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTfoot2_f.first) - c.body.frame(c.frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTfoot1_f.second));

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTreg_f.first) - c.body.frame(c.frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTreg_f.first, c.mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFTreg_f.second));
										}
									}
									//右
									if (Drawparts->tracker_num.size() > 2) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[2]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &mine.pos_RIGHTREG, &mine.mat_RIGHTREG);
										c.mat_RIGHTREG = MATRIX_ref::Axis1(c.mat_RIGHTREG.xvec()*-1.f, c.mat_RIGHTREG.yvec(), c.mat_RIGHTREG.zvec()*-1.f);
										if ((mine.start_c || (ptr_.turn && ptr_.now) != oldv_3_1) && oldv_3_2) {
											mine.mat_RIGHTREG_rep = mine.mat_RIGHTREG;
											if (!mine.start_c) {
												//oldv_3_2 = false;
											}
										}
										oldv_3_1 = ptr_.turn && ptr_.now;
										mine.mat_RIGHTREG =
											MATRIX_ref::RotY(deg2rad(180 - 22 - 10))*
											mine.mat_RIGHTREG_rep.Inverse()*mine.mat_RIGHTREG;
										mine.pos_RIGHTREG = mine.pos_RIGHTREG + (mine.pos - mine.rec_HMD);
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_RIGHTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_RIGHTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする


											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_s.RIGHTreg_f.first) - c.body.frame(c.frame_s.RIGHTfoot2_f.first)).size(), (c.body.frame(c.frame_s.RIGHTfoot2_f.first) - c.body.frame(c.frame_s.RIGHTfoot1_f.first)).size(), (c.body.frame(c.frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTfoot2_f.first) - c.body.frame(c.frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot1_f.second));
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTreg_f.first) - c.body.frame(c.frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTreg_f.first, c.mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHTreg_f.second));
										}

										{
											auto pp = mapparts->map_col_line(mine.body.frame(mine.frame_s.RIGHTreg2_f.first) + VGet(0, 1.8f, 0), mine.body.frame(mine.frame_s.RIGHTreg2_f.first));
											if (pp.HitFlag) {
												mine.pos_RIGHTREG = VECTOR_ref(pp.HitPosition) - (mine.body.frame(mine.frame_s.RIGHTreg2_f.first) - mine.body.frame(mine.frame_s.RIGHTreg_f.first));
											}
										}
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_RIGHTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_RIGHTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_s.RIGHTreg_f.first) - c.body.frame(c.frame_s.RIGHTfoot2_f.first)).size(), (c.body.frame(c.frame_s.RIGHTfoot2_f.first) - c.body.frame(c.frame_s.RIGHTfoot1_f.first)).size(), (c.body.frame(c.frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTfoot2_f.first) - c.body.frame(c.frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot1_f.second));
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTreg_f.first) - c.body.frame(c.frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTreg_f.first, c.mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHTreg_f.second));
										}
									}

								}
								//手
								{
									c.body.get_anime(3).per = 0.f;
									c.body.get_anime(3).time = 0.f;
									//右手
									{
										if (mine.HP == 0) {
											c.mat_RIGHTHAND;//リコイル
											c.pos_RIGHTHAND;
										}
										else {
											Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &c.pos_RIGHTHAND, &c.mat_RIGHTHAND);
											c.pos_RIGHTHAND = c.pos_RIGHTHAND + (c.pos - c.rec_HMD);
											c.mat_RIGHTHAND = MATRIX_ref::Axis1(c.mat_RIGHTHAND.xvec()*-1.f, c.mat_RIGHTHAND.yvec(), c.mat_RIGHTHAND.zvec()*-1.f);
											c.mat_RIGHTHAND = c.mat_RIGHTHAND*MATRIX_ref::RotAxis(c.mat_RIGHTHAND.xvec(), deg2rad(-60));
											c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_RIGHTHAND;//リコイル
										}
										//銃器
										c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
										{
											//基準
											VECTOR_ref tgt_pt = c.obj_gun.frame(c.gun_ptr->frame_s.right_f.first);
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//基準
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
											float cos_t = getcos_tri((c.body.frame(c.frame_s.RIGHThand_f.first) - c.body.frame(c.frame_s.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_s.RIGHTarm2_f.first) - c.body.frame(c.frame_s.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_s.RIGHTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTarm2_f.first) - c.body.frame(c.frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
											}
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHThand_f.first) - c.body.frame(c.frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.RIGHThand_f.first,
												MATRIX_ref::RotY(deg2rad(-10))*
												MATRIX_ref::RotZ(deg2rad(50))*
												MATRIX_ref::RotX(deg2rad(90))*
												c.mat_RIGHTHAND*
												m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHThand_f.second));
										}
										//右人差し指
										c.body.get_anime(0).per = 1.f;
										c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
										c.body.get_anime(9).per = 1.f - c.body.get_anime(5).per;
									}
									//左手
									{
										Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &c.pos_LEFTHAND, &c.mat_LEFTHAND);
										c.pos_LEFTHAND = c.pos_LEFTHAND + (c.pos - c.rec_HMD);
										c.mat_LEFTHAND = MATRIX_ref::Axis1(c.mat_LEFTHAND.xvec()*-1.f, c.mat_LEFTHAND.yvec(), c.mat_LEFTHAND.zvec()*-1.f);
										c.mat_LEFTHAND = c.mat_LEFTHAND*MATRIX_ref::RotAxis(c.mat_LEFTHAND.xvec(), deg2rad(-60));

										{
											float dist_ = (c.pos_LEFTHAND - c.obj_gun.frame(c.gun_ptr->frame_s.left_f.first)).size();
											if (dist_ <= 0.1f && (!c.reloadf || !c.down_mag)) {
												c.LEFT_hand = true;
												c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame_s.left_f.first);
											}
											else {
												c.LEFT_hand = false;
											}
										}
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTHAND;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
											float cos_t = getcos_tri((c.body.frame(c.frame_s.LEFThand_f.first) - c.body.frame(c.frame_s.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_s.LEFTarm2_f.first) - c.body.frame(c.frame_s.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_s.LEFTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTarm2_f.first) - c.body.frame(c.frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
											}

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFThand_f.first) - c.body.frame(c.frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTarm2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_s.LEFThand_f.first, MATRIX_ref::RotZ(deg2rad(-60))* MATRIX_ref::RotX(deg2rad(80))* c.mat_LEFTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFThand_f.second));
										}

									}
								}
							}
							else {
								if (c.HP != 0) {
									{
										VECTOR_ref v_ = c.mat.zvec();
										float x_1 = -sinf(c.body_yrad);
										float y_1 = cosf(c.body_yrad);
										float x_2 = v_.x();
										float y_2 = -v_.z();
										float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
										c.body_yrad += r_ * FRAME_RATE / fps_ / 10.f;
									}
									{
										VECTOR_ref v_ = c.mat.zvec();
										float x_1 = sinf(c.body_xrad);
										float y_1 = -cosf(c.body_xrad);
										float x_2 = -v_.y();
										float y_2 = -std::hypotf(v_.x(), v_.z());
										c.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									}
								}
								//身体
								MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*MATRIX_ref::RotZ(c.body_zrad);
								MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*mg_inv;
								{
									if (c.reloadf) {
										mb_inv = MATRIX_ref::RotZ(c.body_zrad)*MGetIdent();
										m_inv = MATRIX_ref::RotZ(c.body_zrad)* MATRIX_ref::RotX(c.body_xrad)*mg_inv;
									}
									//
									if (c.HP != 0) {
										c.body.SetMatrix(MATRIX_ref::Mtrans(c.pos - c.rec_HMD));
										c.body.SetFrameLocalMatrix(c.frame_s.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(c.frame_s.bodyg_f.second));
										c.body.SetFrameLocalMatrix(c.frame_s.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(c.frame_s.bodyb_f.second));
										c.body.SetFrameLocalMatrix(c.frame_s.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(c.frame_s.body_f.second));
									}
									else {
										m_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
										c.body.SetMatrix(m_inv*MATRIX_ref::Mtrans(c.pos - c.rec_HMD));
									}
								}

								//頭部
								if (c.HP != 0) {
									c.body.SetFrameLocalMatrix(c.frame_s.head_f.first, c.mat*m_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.head_f.second));
									if (c.reloadf) {
										c.body.frame_reset(c.frame_s.head_f.first);
									}
								}
								//足
								{
									auto ratio_t = c.add_pos.size() / c.speed;
									if (c.HP == 0) {
									}
									else {
										if (c.running) {
											easing_set(&c.body.get_anime(8).per, 0.f, 0.95f);
											easing_set(&c.body.get_anime(7).per, 0.f, 0.95f);
											easing_set(&c.body.get_anime(2).per, 1.f*ratio_t, 0.95f);
											easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
										}
										else if (c.ads.first) {
											easing_set(&c.body.get_anime(2).per, 0.f, 0.95f);
											if (!c.squat.first) {
												easing_set(&c.body.get_anime(1).per, 0.5f*ratio_t, 0.95f);
												easing_set(&c.body.get_anime(8).per, 0.f, 0.9f);
												easing_set(&c.body.get_anime(7).per, 0.f, 0.9f);
											}
											else {
												easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
												easing_set(&c.body.get_anime(8).per, 0.5f*ratio_t, 0.9f);
												easing_set(&c.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f);
											}
										}
										else {
											easing_set(&c.body.get_anime(2).per, 0.f, 0.95f);
											if (!c.squat.first) {
												easing_set(&c.body.get_anime(1).per, 1.f*ratio_t, 0.95f);
												easing_set(&c.body.get_anime(8).per, 0.f, 0.9f);
												easing_set(&c.body.get_anime(7).per, 0.f, 0.9f);
											}
											else {
												easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
												easing_set(&c.body.get_anime(8).per, 1.f*ratio_t, 0.9f);
												easing_set(&c.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f);
											}
											easing_set(&c.body.get_anime(0).per, 0.f, 0.95f);
										}
										c.body.get_anime(1).time += 30.f / fps_;
										if (c.body.get_anime(1).time >= c.body.get_anime(1).alltime) {
											c.body.get_anime(1).time = 0.f;
										}
										c.body.get_anime(2).time += 30.f / fps_;
										if (c.body.get_anime(2).time >= c.body.get_anime(2).alltime) {
											c.body.get_anime(2).time = 0.f;
										}
										c.body.get_anime(8).time += 30.f / fps_ * ((c.body.get_anime(8).alltime / 30.f) / c.gun_ptr->reload_time);
										if (c.body.get_anime(8).time >= c.body.get_anime(8).alltime) {
											c.body.get_anime(8).time = 0.f;
										}
									}
								}
								//視点
								{
									VECTOR_ref pv = VGet(0, 0, 0);
									if (c.gun_ptr->frame_s.site_f.first != INT_MAX) {
										pv = c.gun_ptr->frame_s.site_f.second;
									}
									if (c.ads.first) {
										easing_set(&c.gunpos, VGet(-0.f, 0.f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.001f*c.body.get_anime(1).per, -0.315f), 0.75f);
									}
									else {
										if (c.running) {
											easing_set(&c.gunpos, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f);
										}
										else {
											easing_set(&c.gunpos, VGet(-0.1f, -0.05f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.002f*c.body.get_anime(1).per, -0.335f), 0.75f);
										}
									}
								}
								//手
								{
									c.body.frame_reset(c.frame_s.RIGHTarm1_f.first);
									c.body.frame_reset(c.frame_s.RIGHTarm2_f.first);
									c.body.frame_reset(c.frame_s.RIGHThand_f.first);
									c.body.frame_reset(c.frame_s.LEFTarm1_f.first);
									c.body.frame_reset(c.frame_s.LEFTarm2_f.first);
									c.body.frame_reset(c.frame_s.LEFThand_f.first);

									if (c.HP != 0) {
										if (c.running) {
											if (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
												c.body.get_anime(3).per = 1.f;
												c.body.get_anime(3).time += 30.f / fps_ * ((c.body.get_anime(3).alltime / 30.f) / c.gun_ptr->reload_time);
												if (c.body.get_anime(3).time >= c.body.get_anime(3).alltime) {
													c.body.get_anime(3).time = 0.f;
												}

												c.body.get_anime(6).per = 0.f;
												c.body.get_anime(6).time = 0.f;
											}
											else {
												c.body.get_anime(3).per = 0.f;
												c.body.get_anime(3).time = 0.f;

												c.body.get_anime(6).per = 1.f;
												c.body.get_anime(6).time += 30.f / fps_;
												if (c.body.get_anime(6).time >= c.body.get_anime(6).alltime) {
													c.body.get_anime(6).time = 0.f;
												}
											}
										}
										else {
											c.body.get_anime(6).per = 0.f;
											c.body.get_anime(6).time = 0.f;
											if (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
												c.body.get_anime(3).per = 1.f;
												c.body.get_anime(3).time += 30.f / fps_ * ((c.body.get_anime(3).alltime / 30.f) / c.gun_ptr->reload_time);
												if (c.body.get_anime(3).time >= c.body.get_anime(3).alltime) {
													c.body.get_anime(3).time = 0.f;
												}
											}
											else {
												c.body.get_anime(3).per = 0.f;
												c.body.get_anime(3).time = 0.f;
												//右手
												{
													//視点を一時取得
													c.pos_HMD = (c.body.frame(c.frame_s.RIGHTeye_f.first) + (c.body.frame(c.frame_s.LEFTeye_f.first) - c.body.frame(c.frame_s.RIGHTeye_f.first))*0.5f) - c.pos;
													//銃器
													c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat;//リコイル
													c.pos_RIGHTHAND = (c.pos + c.pos_HMD - c.rec_HMD) + (MATRIX_ref::Vtrans(c.gunpos, c.mat_RIGHTHAND) - c.rec_HMD);
													c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
													//基準
													VECTOR_ref tgt_pt = c.obj_gun.frame(c.gun_ptr->frame_s.right_f.first);
													VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
													VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
													float cos_t = getcos_tri((c.body.frame(c.frame_s.RIGHThand_f.first) - c.body.frame(c.frame_s.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_s.RIGHTarm2_f.first) - c.body.frame(c.frame_s.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_s.RIGHTarm1_f.first) - tgt_pt).size());
													VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
													//上腕
													c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
													MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHTarm2_f.first) - c.body.frame(c.frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
													c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm1_f.second));
													//下腕
													c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_s.RIGHTarm2_f.second));
													MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_s.RIGHThand_f.first) - c.body.frame(c.frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_s.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
													c.body.SetFrameLocalMatrix(c.frame_s.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.RIGHTarm2_f.second));
													//手
													c.body.SetFrameLocalMatrix(c.frame_s.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* c.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.RIGHThand_f.second));
												}
												//左手
												{
													if (c.down_mag) {
														c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first) + c.mat_RIGHTHAND.yvec()*-0.05f;
													}
													else {
														c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame_s.left_f.first);
													}
													if (!c.reloadf || !c.down_mag) {
														c.LEFT_hand = true;
														c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame_s.left_f.first);
													}
													else {
														c.LEFT_hand = false;
													}
													c.mat_LEFTHAND = c.mat;

													{
														VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((c.pos_LEFTHAND - c.body.frame(c.frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
														VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
														float cos_t = getcos_tri((c.body.frame(c.frame_s.LEFThand_f.first) - c.body.frame(c.frame_s.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_s.LEFTarm2_f.first) - c.body.frame(c.frame_s.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_s.LEFTarm1_f.first) - (c.pos + c.pos_LEFTHAND - (c.pos - c.rec_HMD))).size());
														VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
														//上腕
														c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
														MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
															MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFTarm2_f.first) - c.body.frame(c.frame_s.LEFTarm1_f.first), m_inv.Inverse()),
															vec_t
														);
														c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTarm1_f.second));
														//下腕
														c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_s.LEFTarm2_f.second));
														MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
															MATRIX_ref::Vtrans(c.body.frame(c.frame_s.LEFThand_f.first) - c.body.frame(c.frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
															MATRIX_ref::Vtrans(c.pos_LEFTHAND - c.body.frame(c.frame_s.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
														);
														c.body.SetFrameLocalMatrix(c.frame_s.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_s.LEFTarm2_f.second));
														//手
														c.body.SetFrameLocalMatrix(c.frame_s.LEFThand_f.first,
															MATRIX_ref::RotZ(deg2rad(-60))*
															MATRIX_ref::RotX(deg2rad(80))*
															c.mat_LEFTHAND*
															m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_s.LEFThand_f.second));
													}
												}
											}
										}
										//右人差し指
										{
											c.body.get_anime(0).per = 1.f;
											c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
											c.body.get_anime(9).per = 1.f - c.body.get_anime(5).per;
										}
										c.add_RIGHTHAND = c.add_pos;
									}
									else {
										//銃器

										c.pos_RIGHTHAND += c.add_RIGHTHAND;
										c.add_RIGHTHAND.yadd(M_GR / std::powf(fps_, 2.f));

										auto pp = mapparts->map_col_line(c.pos_RIGHTHAND + VGet(0, 1.f, 0), c.pos_RIGHTHAND - VGet(0, 0.05f, 0));
										if (pp.HitFlag) {
											c.pos_RIGHTHAND = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
											c.mat_RIGHTHAND *= MATRIX_ref::RotVec2(c.mat_RIGHTHAND.xvec(), VECTOR_ref(pp.Normal));
											easing_set(&c.add_RIGHTHAND, VGet(0, 0, 0), 0.8f);
										}

										c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
									}
								}
							}
							c.body.work_anime();

							//

							c.frame_s.copy_frame(c.body, c.colframe_, &c.col);
							c.col.work_anime();
							c.col.RefreshCollInfo(-1, 0);
							c.col.RefreshCollInfo(-1, 1);
							c.col.RefreshCollInfo(-1, 2);
							//
							if (!(Drawparts->use_vr && (&c == &mine))) {
								//視点取得
								c.pos_HMD = (c.body.frame(c.frame_s.RIGHTeye_f.first) + (c.body.frame(c.frame_s.LEFTeye_f.first) - c.body.frame(c.frame_s.RIGHTeye_f.first))*0.5f) - c.pos;
								//銃器
								if (c.HP != 0) {
									if (c.running || (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1)) {
										auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.frame_s.RIGHThand2_f.first);
										c.pos_RIGHTHAND = c.body.frame(c.frame_s.RIGHThand_f.first);
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND - c.obj_gun.frame(c.gun_ptr->frame_s.right_f.first) + c.pos_RIGHTHAND));
										//
										c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.frame_s.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.frame_s.LEFThand2_f.first)).Inverse());
										c.pos_LEFTHAND = c.body.frame(c.frame_s.LEFThand_f.first) + c.mat_LEFTHAND.yvec()*0.1f;
									}
									else {
										auto mat_T = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat;//リコイル
										c.pos_RIGHTHAND = c.pos + c.pos_HMD + MATRIX_ref::Vtrans(c.gunpos, mat_T);
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
									}
								}
							}
							//銃共通
							{
								if (c.obj_gun.get_anime(3).per == 1.f) {
									c.audio.slide.play_3D(c.pos_RIGHTHAND, 15.f);
								}
								c.obj_gun.get_anime(3).per = std::max(c.obj_gun.get_anime(3).per - 12.f / fps_, 0.f);
							}
						}
						//
						for (auto& c : chara) {
							//射撃関連
							{
								{
									//複座
									easing_set(&c.vecadd_RIGHTHAND, c.vecadd_RIGHTHAND_p, 0.75f);
									easing_set(&c.vecadd_RIGHTHAND_p, VGet(0, 0, 1.f), 0.9f);
									//リコイル
									if (c.gunf) {
										if (c.gun_stat[c.gun_ptr->id].ammo_cnt >= 1) {
											c.obj_gun.get_anime(0).per = 1.f;
											c.obj_gun.get_anime(1).per = 0.f;
											c.obj_gun.get_anime(0).time += 60.f / fps_;
											if (c.obj_gun.get_anime(0).time >= c.obj_gun.get_anime(0).alltime) {
												c.obj_gun.get_anime(0).time = 0.f;
												c.gunf = false;
											}
										}
										else {
											c.obj_gun.get_anime(1).per = 1.f;
											c.obj_gun.get_anime(0).per = 0.f;
											c.obj_gun.get_anime(1).time += 60.f / fps_;
											if (c.obj_gun.get_anime(1).time >= c.obj_gun.get_anime(1).alltime) {
												c.obj_gun.get_anime(1).time = c.obj_gun.get_anime(1).alltime;
												c.gunf = false;
											}
										}

									}
									//マガジン排出
									if (c.obj_gun.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
										//音
										c.audio.mag_down.play_3D(c.pos_RIGHTHAND, 15.f);
										//バイブレーション　バッテリー消費が激しいためコメントアウト
										/*
											Drawparts->Haptic(Drawparts->get_hand1_num(), unsigned short(60000));
										*/
										//弾数
										auto dnm = (c.gun_stat[c.gun_ptr->id].ammo_cnt >= 1) ? c.gun_stat[c.gun_ptr->id].ammo_cnt - 1 : 0;
										//マガジン排出
										c.gun_stat[c.gun_ptr->id].mag_release();
										c.reload_cnt = 0.f;
										//マガジン排出
										bool tt = false;
										for (auto& g : this->item) {
											if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
												tt = true;
												g.Set_item(c.gun_ptr->magazine, c.pos_mag, c.mat_mag);
												g.add = (c.obj_gun.frame(c.gun_ptr->frame_s.magazine2_f.first) - c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
												g.magazine.cap = dnm;
												g.del_timer = 0.f;
												break;
											}
										}
										if (!tt) {
											this->item.resize(this->item.size() + 1);
											auto& g = this->item.back();
											g.id = this->item.size() - 1;
											g.Set_item(c.gun_ptr->magazine, c.pos_mag, c.mat_mag);
											g.add = (c.obj_gun.frame(c.gun_ptr->frame_s.magazine2_f.first) - c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
											g.magazine.cap = dnm;
											g.del_timer = 0.f;
										}
										//
										c.reloadf = true;
									}

									if (c.delete_item.second == 1) {
										//medkit排出
										bool tt = false;
										for (auto& g : this->item) {
											if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
												tt = true;
												g.Set_item(&this->meds_data[0], c.pos_mag, c.mat_mag);
												g.add = (c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first-1) - c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first)).Norm()*-5.f / fps_;//排莢ベクトル
												break;
											}
										}
										if (!tt) {
											this->item.resize(this->item.size() + 1);
											auto& g = this->item.back();
											g.id = this->item.size() - 1;
											g.Set_item(&this->meds_data[0], c.pos_mag, c.mat_mag);
											g.add = (c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first-1) - c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first)).Norm()*-5.f / fps_;//排莢ベクトル
										}
									}
									//マガジン挿入
									if (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
										if (Drawparts->use_vr) {
											if (c.reload_cnt < c.gun_ptr->reload_time) {
												c.down_mag = false;
											}
											if (c.down_mag) {
												if ((c.obj_mag.frame(3) - c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first)).size() <= 0.05f) {
													c.obj_gun.get_anime(1).time = 0.f;
													c.obj_gun.get_anime(0).per = 1.f;
													c.obj_gun.get_anime(1).per = 0.f;
													if (c.gun_stat[c.gun_ptr->id].ammo_cnt == 0) {
														c.obj_gun.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos_RIGHTHAND, 15.f);
													c.gun_stat[c.gun_ptr->id].mag_slide();//チャンバーに装填
													c.reloadf = false;
												}
												c.pos_mag = c.pos_LEFTHAND;
												c.mat_mag = c.obj_mag.GetFrameLocalMatrix(3)* (c.mat_LEFTHAND*MATRIX_ref::RotVec2(c.mat_LEFTHAND.yvec(), (c.obj_gun.frame(c.gun_ptr->frame_s.magazine_f.first) - c.pos_LEFTHAND)));
											}
										}
										else {
											if (c.down_mag) {
												if (c.reload_cnt > c.gun_ptr->reload_time) {
													c.obj_gun.get_anime(1).time = 0.f;
													c.obj_gun.get_anime(0).per = 1.f;
													c.obj_gun.get_anime(1).per = 0.f;
													if (c.gun_stat[c.gun_ptr->id].ammo_cnt == 0) {
														c.obj_gun.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos_RIGHTHAND, 15.f);
													c.gun_stat[c.gun_ptr->id].mag_slide();//チャンバーに装填
													c.reloadf = false;
												}
												c.pos_mag = c.pos_LEFTHAND;
												c.mat_mag = c.mat_LEFTHAND;
											}
										}
										c.reload_cnt += 1.f / fps_;//挿入までのカウント
										//
									}
									else {
										c.down_mag = false;
										c.pos_mag = c.obj_gun.frame(c.gun_ptr->frame_s.magazine2_f.first);
										c.mat_mag = c.mat_RIGHTHAND;
									}
									//セレクター
									easing_set(&c.obj_gun.get_anime(4).per, float(c.selkey.first), 0.5f);
									if (c.selkey.second == 1) {
										++c.gun_stat[c.gun_ptr->id].select %= c.gun_ptr->select.size();
									}
									//セフティ
									//easing_set(&c.obj_gun.get_anime(4).per, float(0.f), 0.5f);
									//射撃
									if (!c.gunf && c.gun_stat[c.gun_ptr->id].ammo_cnt >= 1) {
										if (c.gun_ptr->select[c.gun_stat[c.gun_ptr->id].select] == 2) {//フルオート用
											c.trigger.second = 0;
										}
									}
									c.trigger.get_in(c.obj_gun.get_anime(2).per >= 0.5f);
									if (c.trigger.second == 1) {
										c.audio.trigger.play_3D(c.pos_RIGHTHAND, 5.f);
										if (!c.gunf && c.gun_stat[c.gun_ptr->id].ammo_cnt >= 1) {
											c.gunf = true;
											//弾数管理

											c.gun_stat[c.gun_ptr->id].mag_shot(c.reloadf);
											//持ち手を持つとココが相殺される
											c.vecadd_RIGHTHAND_p = MATRIX_ref::Vtrans(c.vecadd_RIGHTHAND_p,
												MATRIX_ref::RotY(deg2rad(float((int32_t)(c.gun_ptr->recoil_xdn*100.f) + GetRand((int32_t)((c.gun_ptr->recoil_xup - c.gun_ptr->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float((int32_t)(c.gun_ptr->recoil_ydn*100.f) + GetRand((int32_t)((c.gun_ptr->recoil_yup - c.gun_ptr->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
											//弾
											c.bullet[c.use_bullet].set(&c.gun_ptr->ammo[0], c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first), c.mat_RIGHTHAND.zvec()*-1.f);
											++c.use_bullet %= c.bullet.size();//次のIDへ
											//エフェクト
											c.effcs[ef_fire].set(c.obj_gun.frame(c.gun_ptr->frame_s.mazzule_f.first), c.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);
											//サウンド
											c.audio.shot.play_3D(c.pos_RIGHTHAND, 100.f);
											c.audio.slide.play_3D(c.pos_RIGHTHAND, 15.f);
										}
									}
								}
								c.obj_mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj_gun.work_anime();
								for (auto& a : c.bullet) {
									a.update(&c, &chara, mapparts);
								}
								//
								for (auto& t : c.effcs) {
									const size_t index = &t - &c.effcs[0];
									if (index != ef_smoke) {
										t.put(Drawparts->get_effHandle((int32_t)(index)));
									}
								}
								for (auto& t : c.effcs_gndhit) {
									t.put(Drawparts->get_effHandle(ef_gndsmoke));
								}
							}
							//アイテム関連
							{
								//拾う
								c.canget_gunitem = false;
								c.canget_magitem = false;
								c.canget_meditem = false;
								if (&c == &chara[0]) {
									for (auto& g : this->item) {
										g.update(item, mapparts);
									}
								}
								for (auto& g : this->item) {
									g.Get_item_2(c, mapparts);
								}

								for (auto& g : this->item) {
									if (g.ptr_mag != nullptr && g.magazine.cap == 0) {
										g.del_timer += 1.f / fps_;
									}
								}
								while (true) {
									bool p = false;
									size_t id = 0;
									for (auto& g : this->item) {
										if (g.ptr_mag != nullptr) {
											if (g.del_timer >= 5.f) {
												g.Delete_item();
												id = &g - &this->item[0];
												p = true;
											}
										}
									}
									if (p) {
										this->item.erase(this->item.begin() + id);
									}
									else {
										break;
									}
								}
							}
						}
						//物理演算、アニメーション
						for (auto& c_ : chara) {
							if (c_.start_c) {
								c_.body.PhysicsResetState();
								c_.start_c = false;
							}
							else {
								if (c_.start_b) {
									c_.body.PhysicsResetState();
									c_.start_b = false;
								}
								else {
									c_.body.PhysicsCalculation(1000.f / fps_);
								}
							}
						}
						//campos,camvec,camupの指定
						{
							//auto& cc = chara[1];
							auto& cc = mine;
							if (cc.HP != 0) {
								camera_main.campos = cc.pos + cc.pos_HMD - cc.rec_HMD;
								camera_main.camvec = camera_main.campos + cc.mat.zvec()*(Drawparts->use_vr ? 1.f : -1.f);
								camera_main.camup = cc.mat.yvec();
							}
							else {
								if (cc.death_timer <= 2.5f) {
									easing_set(&camera_main.camvec, chara[cc.death_id].pos + chara[cc.death_id].pos_HMD - chara[cc.death_id].rec_HMD, 0.9f);
									auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
									easing_set(&camera_main.campos, cc.pos + cc.pos_HMD - cc.rec_HMD + VGet(-5.f*sin(rad), 2.f, -5.f*cos(rad)), 0.9f);
									camera_main.camup = VGet(0, 1.f, 0);
								}
								else {
									camera_main.campos = (cc.pos + cc.pos_HMD - cc.rec_HMD) + cc.mat.zvec()*5.f;
									camera_main.camvec = cc.pos + cc.pos_HMD - cc.rec_HMD;
									camera_main.camup = cc.mat.yvec();
								}
								mapparts->map_col_nearest(camera_main.camvec, &camera_main.campos);
							}
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						UpdateEffekseer3D();
						//VR空間に適用
						Drawparts->Move_Player();
						//hit時に何かする部分
						for (auto& a : mine.bullet) {
							if (a.hit) {
								a.hit = false;
							}
						}
						//ルール保存
						UIparts->set_rule(ruleparts->getready(), ruleparts->gettimer());

						//1P描画
						{
							//影用意
							Drawparts->Ready_Shadow(camera_main.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
								for (auto& g : this->item) { g.Draw_item(); }
							},
								VGet(2.f, 2.5f, 2.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//書き込み
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(mine, Drawparts->use_vr);
								}
							}
							{
								if (this->TPS.first) {
									GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camera_TPS.campos, camera_TPS.camvec, camera_TPS.camup, camera_TPS.fov, camera_TPS.near_, camera_TPS.far_);
								}
								else {
									GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camera_main.campos, camera_main.camvec, camera_main.camup, camera_main.fov, camera_main.near_, camera_main.far_);
								}
								for (auto& c : chara) {
									auto ttt = c.body.GetMatrix().pos();
									if (CheckCameraViewClip_Box((ttt + VGet(-0.3f, 0, -0.3f)).get(), (ttt + VGet(0.3f, 1.8f, 0.3f)).get())) {
										c.start_b = true;
										continue;
									}
								}
							}
							//VRに移す
							Drawparts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									Hostpassparts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//最終描画
									Hostpassparts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									//main
									Hostpassparts->get_main().DrawGraph(0, 0, true);
									//UI
									if (Drawparts->use_vr) {
										SetCameraNearFar(0.01f, 2.f);
										SetUseZBuffer3D(FALSE);												//zbufuse
										SetWriteZBuffer3D(FALSE);											//zbufwrite
										{
											DrawBillboard3D((camera_main.campos + (camera_main.camvec - camera_main.campos).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, this->UI_Screen.get(), TRUE);
										}
										SetUseZBuffer3D(TRUE);												//zbufuse
										SetWriteZBuffer3D(TRUE);											//zbufwrite
									}
									else {
										this->UI_Screen.DrawGraph(0, 0, TRUE);
									}
									//UI2
									UIparts->item_draw(chara, this->item, camera_main.campos, Drawparts->use_vr);
								}
							}, camera_main);
						}
						if (!this->TPS.first) {
							//2P描画
							if (Drawparts->use_vr) {
								auto& ct = chara[sel_cam];
								//cam_s.cam
								camera_sub.campos = ct.pos + ct.pos_HMD - ct.rec_HMD;
								camera_sub.camvec = camera_sub.campos + ct.mat.zvec()*-1.f;
								camera_sub.camup = ct.mat.yvec();

								//影用意
								Drawparts->Ready_Shadow(camera_sub.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
									for (auto& g : this->item) { g.Draw_item(); }
								},
									VGet(5.f, 2.5f, 5.f),
									VGet(5.f, 2.5f, 5.f)
									);
								//書き込み
								{
									this->UI_Screen2.SetDraw_Screen();
									{
										UIparts->set_draw(ct, false);
									}
									//被写体深度描画
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_sub);
									//最終描画
									Hostpass2parts->MAIN_draw();
								}
								//Screen2に移す
								outScreen2.SetDraw_Screen(camera_sub.campos, camera_sub.camvec, camera_sub.camup, camera_sub.fov, camera_sub.near_, camera_sub.far_);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
									//UI
									this->UI_Screen2.DrawGraph(0, 0, true);
									//UI2
									UIparts->item_draw(chara, this->item, camera_sub.campos, false);
								}
							}
						}
						//ディスプレイ描画
						{
							//ミニマップ
							{
								auto* cc = &mine;
								if (this->TPS.first) {
									cc = &chara[sel_cam];
								}
								else {
									if (Drawparts->use_vr) {
										cc = &chara[sel_cam];
									}
									else {
										cc = &chara[0];
									}
								}
								minimapparts->set(chara, *cc, mapparts);
							}
							//TPS視点
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							if (this->TPS.first) {
								{
									//cam
									for (int i = 0; i < std::min<size_t>(chara.size(), 10); i++) {
										if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
											sel_cam = i;
										}
									}
									//pos
									{
										if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
											camera_TPS.campos.x(camera_TPS.campos.x() - 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.z(camera_TPS.campos.z() + 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
											camera_TPS.campos.x(camera_TPS.campos.x() + 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.z(camera_TPS.campos.z() - 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_UP) != 0) {
											camera_TPS.campos.z(camera_TPS.campos.z() + 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.x(camera_TPS.campos.x() + 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
											camera_TPS.campos.z(camera_TPS.campos.z() - 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.x(camera_TPS.campos.x() - 10.f / fps_ * sin(yr_cam));
										}
										camera_TPS.campos.x(std::clamp(camera_TPS.campos.x(), mapparts->map_col_get().mesh_minpos(0).x(), mapparts->map_col_get().mesh_maxpos(0).x()));
										camera_TPS.campos.z(std::clamp(camera_TPS.campos.z(), mapparts->map_col_get().mesh_minpos(0).z(), mapparts->map_col_get().mesh_maxpos(0).z()));
									}
									//rad
									{
										VECTOR_ref vec_2 = (chara[sel_cam].body.frame(chara[sel_cam].frame_s.head_f.first) - camera_TPS.campos).Norm();
										VECTOR_ref vec_z = (camera_TPS.camvec - camera_TPS.campos).Norm();
										VECTOR_ref vec_x = vec_z.cross(camera_TPS.camup);

										xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50))*0.1f;
										yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50))*0.1f;
										xr_cam = std::clamp(xr_cam, deg2rad(-89), deg2rad(89));
									}
									//
									camera_TPS.camvec = camera_TPS.campos + MATRIX_ref::Vtrans(VGet(0, 0, 1), MATRIX_ref::RotX(xr_cam)*MATRIX_ref::RotY(yr_cam));
									camera_TPS.camup = VGet(0, 1.f, 0);
								}
								//影用意
								Drawparts->Ready_Shadow(camera_TPS.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
									for (auto& g : this->item) { g.Draw_item(); }
								},
									VGet(5.f, 2.5f, 5.f),
									VGet(5.f, 2.5f, 5.f)
									);
								//書き込み
								{
									//被写体深度描画
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_TPS);
									//最終描画
									Hostpass2parts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
									minimapparts->draw(64, 64);
								}
							}
							else {//FPS視点
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									if (Drawparts->use_vr) {
										outScreen2.DrawGraph(0, 0, false);
									}
									else {
										Drawparts->outScreen[0].DrawGraph(0, 0, false);
									}

									minimapparts->draw(64, 64);
								}
							}
							//デバッグ
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					ruleparts->update();
					//画面の反映
					Drawparts->Screen_Flip(waits);
					//終了判定
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
					//
					for (auto& c : chara) {
						for (auto& t : c.effcs) {
							t.put_end();
						}
						for (auto& t : c.effcs_gndhit) {
							t.put_end();
						}
					}
					//
				}
			}
			//解放
			{
				for (auto& c : chara) {
					c.Delete_chara();
				}
				chara.clear();
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
			//
		} while (ProcessMessage() == 0 && this->ending);
	}
};