#pragma once
class main_c : Mainclass {
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	float fov_pc = 45.f;

	DXDraw::cam_info cam_easy;
	DXDraw::cam_info cam_easy2;
	DXDraw::cam_info cam_easy3;

	GraphHandle outScreen2;
	GraphHandle UI_Screen;																									//描画スクリーン
	GraphHandle UI_Screen2;																									//描画スクリーン
	//操作
	VECTOR_ref campos_TPS;			//カメラ
	switchs TPS;					//操作スイッチ
	//データ
	MV1 body_obj;					//身体モデル
	MV1 body_col;					//身体モデル
	std::vector<Guns>  gun_data;		//GUNデータ
	std::vector<Mags> mag_data;		//GUNデータ
	std::vector<Chara> chara;		//キャラ
	std::vector<Items> item_data;	//拾えるアイテム
	//設定
	bool oldv = false;
	bool oldv_1 = false;
	bool oldv_2 = false;
	bool oldv_2_2 = false;
	bool oldv_3 = false;
	bool oldv_3_2 = false;

	bool start_c = true;
	bool ending = true;

	float xr_cam = 0.f;
	float yr_cam = 0.f;
	GraphHandle UI_player;																									//描画スクリーン
	GraphHandle UI_minimap;																									//描画スクリーン
	int sel_cam = 0;
	//
public:
	main_c() {
		//設定読み込み
		{
			SetOutApplicationLogValidFlag(FALSE);	//log
			int mdata = FileRead_open("data/setting.txt", FALSE);
			dof_e = getparams::_bool(mdata);
			bloom_e = getparams::_bool(mdata);
			shadow_e = getparams::_bool(mdata);
			useVR_e = getparams::_bool(mdata);
			fov_pc = getparams::_float(mdata);
			FileRead_close(mdata);
			SetOutApplicationLogValidFlag(TRUE);	//log
		}
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, useVR_e, shadow_e);							//汎用
		auto UIparts = std::make_unique<UI>();																		//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);														//デバッグ
		auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->disp_x, Drawparts->disp_y);//ホストパスエフェクト(VR、フルスクリーン共用)
		UI_Screen = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y, true);									//VR、フルスクリーン共用UI
		auto Hostpass2parts = std::make_unique<HostPassEffect>(dof_e, bloom_e, deskx, desky);						//ホストパスエフェクト(フルスクリーン向け)
		UI_Screen2 = GraphHandle::Make(deskx, desky, true);															//フルスクリーン向けUI
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		//model
		MV1::Load("data/model/body/model.mv1", &this->body_obj, true);												//身体
		MV1::Load("data/model/body/col.mv1", &this->body_col, true);												//身体col
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS用描画スクリーン
		UI_minimap = GraphHandle::Make(300, 300, true);
		UI_player = GraphHandle::Load("data/UI/player.bmp");
		auto font = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
		//MAGデータ
		{
			this->mag_data.resize(1);
			this->mag_data[0].mod.set("data/mag/", "GUN");
		}
		//GUNデータ
		{
			this->gun_data.resize(1);
			this->gun_data[0].mod.set("data/gun/", "GUN");
		}
		UIparts->load_window("銃モデル");						//ロード画面1
		//MAGデータ取得2
		for (auto& g : this->mag_data) {
			g.id = &g - &this->mag_data[0];
			g.set_data();
		}
		//GUNデータ取得2
		for (auto& g : this->gun_data) {
			g.id = &g - &this->gun_data[0];
			g.set_data(this->mag_data);
		}
		UIparts->load_window("銃データ");						//ロード画面2
		do {
			//キャラ設定
			size_t sel_g = 0;
			chara.resize(4);
			auto& mine = chara[0];
			//自機セット
			mine.set(this->gun_data, sel_g, this->body_obj, this->body_col);
			mine.spawn(VGet(8.0f, 2.0f, -8.f), MATRIX_ref::RotY(DX_PI_F * 2));
			//その他
			chara[1].set(this->gun_data, sel_g, this->body_obj, this->body_col);
			chara[1].spawn(VGet(8.0f, 2.0f, 8.f), MATRIX_ref::RotY(DX_PI_F * 2));

			chara[2].set(this->gun_data, sel_g, this->body_obj, this->body_col);
			chara[2].spawn(VGet(-8.0f, 2.0f, 8.f), MATRIX_ref::RotY(DX_PI_F * 3 / 2));

			chara[3].set(this->gun_data, sel_g, this->body_obj, this->body_col);
			chara[3].spawn(VGet(-8.0f, 2.0f, -8.f), MATRIX_ref::RotY(DX_PI_F * 3 / 2));

			//マップ読み込み
			mapparts->Ready_map("data/map_new");			//mapparts->Ready_map("data/new");
			UIparts->load_window("マップ");
			mapparts->Set_map("data/maps/set.txt", this->item_data, this->gun_data);
			//ライティング
			Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {
				mapparts->map_get().DrawMesh(0);
				mapparts->map_get().DrawMesh(1);
			});
			//描画するものを指定する(仮)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					mapparts->map_get().DrawMesh(0);
					mapparts->map_get().DrawMesh(1);
					SetFogEnable(FALSE);
					mapparts->map_get().DrawMesh(2);
					SetFogEnable(TRUE);

					for (auto& c : this->chara) {
						c.Draw_chara();
						{
							VECTOR_ref startpos = c.obj_gun.frame(c.gun_ptr->frame[2].first);
							VECTOR_ref endpos = startpos - c.obj_gun.GetMatrix().zvec()*100.f;
							while (true) {
								auto p = mapparts->map_col_line(startpos, endpos, 0);
								if (p.HitFlag) {
									if (endpos == p.HitPosition) {
										break;
									}
									endpos = p.HitPosition;
								}
								else {
									break;
								}
							}
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
							DrawLine3D(startpos.get(), endpos.get(), GetColor(255, 0, 0));
							SetUseLighting(FALSE);
							SetFogEnable(FALSE);
							DrawSphere3D(endpos.get(), std::clamp(powf((endpos - GetCameraPosition()).size() + 0.5f, 2)*0.002f, 0.001f, 0.05f), 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
							SetUseLighting(TRUE);
							SetFogEnable(TRUE);
						}
					}
					for (auto& g : this->item_data) {
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
					c.xrad_p = 0.f;
					c.ads.ready(false);
					c.running = false;
					c.squat.ready(false);
					c.start_b = false;
					c.start_c = true;
					{
						int now = -1;
						auto poss = c.pos + c.pos_HMD - c.rec_HMD;
						auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
						for (auto& ww : c.wayp_pre) {
							ww = now;
						}
						for (auto& w : mapparts->get_waypoint()) {
							auto id = &w - &mapparts->get_waypoint()[0];
							bool tt = true;
							for (auto& ww : c.wayp_pre) {
								if (id == ww) {
									tt = false;
								}
							}
							if (tt) {
								if (tmp.size() >= (w - poss).size()) {
									auto p = mapparts->map_col_line(w + VGet(0, 0.75f, 0), poss + VGet(0, 0.75f, 0), 0);
									if (p.HitFlag == FALSE) {
										tmp = (w - poss);
										now = int(id);
									}
								}
							}
						}
						if (now != -1) {
							for (auto& w : c.wayp_pre) {
								w = now;
							}
						}
					}
				}
				SetMousePoint(deskx / 2, desky / 2);

				cam_easy.fov = deg2rad(Drawparts->use_vr ? 90 : 45);	//
				cam_easy.near_ = 0.1f;
				cam_easy.far_ = 100.f;
				cam_easy2.fov = deg2rad(45);	//
				cam_easy2.near_ = 0.1f;
				cam_easy2.far_ = 100.f;

				cam_easy3.campos = VGet(0, 10, -10);
				cam_easy3.fov = deg2rad(45);
				cam_easy3.near_ = 0.1f;
				cam_easy3.far_ = 100.f;
				//
				while (ProcessMessage() == 0) {
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//座標取得
						if (Drawparts->tracker_num.size() > 0) {
							auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[0]];
							Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &mine.pos_WAIST, &mine.mat_WAIST);
							if (mine.start_c || (ptr_.turn && ptr_.now) != oldv_1) {
								mine.pos_WAIST_rep = VGet(mine.pos_WAIST.x(), 0.f, mine.pos_WAIST.z());
							}
							oldv_1 = ptr_.turn && ptr_.now;
							mine.pos_WAIST = mine.pos_WAIST - mine.pos_WAIST_rep;
						}
						{
							for (auto& c : chara) {
								easing_set(&c.HP_r, float(c.HP), 0.95f);
								if (c.HP == 0) {
									c.spawn(c.spawn_pos, c.spawn_mat);
									{
										int now = -1;
										for (auto& w : c.wayp_pre) {
											w = now;
										}
										auto poss = c.pos + c.pos_HMD - c.rec_HMD;
										auto tmp = mapparts->get_waypoint()[0] - poss;
										for (auto& w : mapparts->get_waypoint()) {
											auto id = &w - &mapparts->get_waypoint()[0];
											bool tt = true;
											for (auto& ww : c.wayp_pre) {
												if (id == ww) {
													tt = false;
												}
											}
											if (tt) {
												if (tmp.size() >= (w - poss).size()) {
													auto p = mapparts->map_col_line(w + VGet(0, 0.75f, 0), poss + VGet(0, 0.75f, 0), 0);
													if (p.HitFlag == FALSE) {
														tmp = (w - poss);
														now = int(id);
													}
												}
											}
										}
										if (now != -1) {
											for (auto& w : c.wayp_pre) {
												w = now;
											}
										}
									}
								}
								if (c.kill_f) {
									c.kill_time -= 1.f / GetFPS();
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
									bool aim = false;
									bool shoot = false;
									bool reload = false;
									c.qkey = false;
									int phase = 0;

									if (c.running) {
										c.squat.first = false;
									}
									if (c.ads.first) {
										c.running = false;
									}
									if (!c.wkey && !c.skey && !c.akey && !c.dkey) {
										c.running = false;
									}
									if (c.running) {
										c.skey = false;
										c.akey = false;
										c.dkey = false;
									}
									//HMD_mat
									{
										int32_t x_m = 0, y_m = 0;
										bool pp = true;
										VECTOR_ref vec_2 = chara[0].body.frame(chara[0].frame_.bodyb_f.first) - c.obj_gun.frame(c.gun_ptr->frame[2].first);
										if (c.ai_time_shoot < 0.f) {
											vec_2 = chara[0].body.frame(chara[0].frame_.head_f.first) - c.obj_gun.frame(c.gun_ptr->frame[2].first);
										}
										for (auto& tgt : chara) {
											if (&c == &tgt) {
												continue;
											}
											auto tmp = tgt.body.frame(tgt.frame_.bodyb_f.first) - c.obj_gun.frame(c.gun_ptr->frame[2].first);
											if (c.ai_time_shoot < 0.f) {
												tmp = tgt.body.frame(tgt.frame_.head_f.first) - c.obj_gun.frame(c.gun_ptr->frame[2].first);
											}

											bool tt = true;
											{
												VECTOR_ref startpos = c.obj_gun.frame(c.gun_ptr->frame[2].first);
												VECTOR_ref endpos = tgt.body.frame(tgt.frame_.bodyb_f.first);
												if (c.ai_time_shoot < 0.f) {
													endpos = tgt.body.frame(tgt.frame_.head_f.first);
												}
												auto p = mapparts->map_col_line(startpos, endpos, 0);
												if (p.HitFlag) {
													tt = false;
												}
											}
											if (!tt) {
												continue;
											}
											if (vec_2.size() >= tmp.size()) {
												vec_2 = tmp;
												pp = false;
											}
										}
										{
											VECTOR_ref vec_x = c.body.GetFrameLocalWorldMatrix(c.frame_.head_f.first).xvec();
											VECTOR_ref vec_y = c.body.GetFrameLocalWorldMatrix(c.frame_.head_f.first).yvec();
											VECTOR_ref vec_z = c.body.GetFrameLocalWorldMatrix(c.frame_.head_f.first).zvec()*-1.f;

											if (pp) {
												phase = 0;
											}
											else {
												if (vec_z.dot(vec_2.Norm()) >= 0) {
													phase = 1;
												}
											}
											if (c.reloadf) {
												phase = 2;
											}

											switch (phase) {
											case 0://通常フェイズ
											{
												c.running = false;
												c.ai_reload = false;
												c.wkey = true;
												c.skey = false;
												c.akey = false;
												c.dkey = false;
												c.squat.first = false;

												if (c.ekey) {
													c.ai_time_e += 1.f / GetFPS();
													if (c.ai_time_e >= 2) {
														c.ekey = false;
														c.ai_time_e = 0.f;
													}
												}

												vec_2 = mapparts->get_waypoint()[c.wayp_pre[0]]- c.body.GetMatrix().pos();
												x_m = -int(vec_x.dot(vec_2) * 80);
												y_m = -int(vec_y.dot(vec_2) * 80);

												//到達時に判断
												if (vec_2.size() <= 0.5f) {
													int now = -1;
													auto poss = c.body.GetMatrix().pos();
													auto tmp = VECTOR_ref(VGet(0,100.f,0));
													for (auto& w : mapparts->get_waypoint()) {
														auto id = &w - &mapparts->get_waypoint()[0];
														bool tt = true;
														for (auto& ww : c.wayp_pre) {
															if (id == ww) {
																tt = false;
															}
														}
														if (tt) {
															if (tmp.size() >= (w - poss).size()) {
																auto p = mapparts->map_col_line(w + VGet(0, 0.75f, 0), poss + VGet(0, 1.f, 0), 0);
																if (p.HitFlag == FALSE) {
																	tmp = (w - poss);
																	now = int(id);
																}
															}
														}
													}
													if (now != -1) {
														for (int i = int(c.wayp_pre.size())-1; i >= 1; i--) {
															c.wayp_pre[i] = c.wayp_pre[i-1];
														}
														c.wayp_pre[0] = now;
													}
												}
											}
											break;
											case 1://戦闘フェイズ
											{
												c.running = false;
												c.ai_reload = false;
												c.wkey = false;
												if (c.ekey) {
													c.ai_time_e += 1.f / GetFPS();
													if (c.ai_time_e >= 2) {
														c.ekey = false;
														c.ai_time_e = 0.f;
													}
												}
												{
													auto poss = c.body.GetMatrix().pos();
													for (auto& w : mapparts->get_waypoint()) {
														if ((w-poss).size()<=0.5f) {
															c.ekey = true;
															c.ai_time_e = 0.f;
															break;
														}
													}
												}

												x_m = -int(vec_x.dot(vec_2) * 40);// +int(-5.f + float(GetRand(5 * 2)));
												y_m = -int(vec_y.dot(vec_2) * 40);// +int(-5.f + float(GetRand(5 * 2)));

												c.ai_time_shoot += 1.f / GetFPS();
												if (c.ai_time_shoot < 0.f) {
													c.akey = false;
													c.dkey = false;
													if (c.ai_time_shoot >= -5) {
														shoot =GetRand(100) <= 10;
														aim = true;
														c.squat.first = true;
													}
												}
												else {
													c.squat.first = false;

													shoot = GetRand(100) <= 15;

													if (c.ai_time_shoot >= GetRand(20) + 5) {
														c.ai_time_shoot = float(-8);
													}
													if (c.ekey && !c.akey) {
														c.akey = true;
														c.ai_time_a = 0.f;
														c.ai_time_a = 0.f;
													}

													if (!c.akey) {
														c.ai_time_d += 1.f / GetFPS();
														if (c.ai_time_d > GetRand(3) + 1) {
															c.akey = true;
															c.ai_time_d = 0.f;
														}
													}
													else {
														c.ai_time_a += 1.f / GetFPS();
														if (c.ai_time_a > GetRand(3) + 1) {
															c.akey = false;
															c.ai_time_a = 0.f;
														}
													}
													c.dkey = !c.akey;
												}
												if (c.gun_stat[c.gun_ptr->id].ammo_cnt == 0 && !c.reloadf) {
													reload = true;
													c.ai_reload = true;
												}
											}
											break;
											case 2://リロードフェイズ
											{
												c.wkey = true;
												c.skey = false;
												c.akey = false;
												c.dkey = false;
												c.running = true;
												c.squat.first = false;

												c.ekey = false;

												if(c.ai_reload) {
													auto pppa = c.wayp_pre[1];
													auto ppp = c.wayp_pre[2];
													for (int i = int(c.wayp_pre.size()) - 1; i >= 2; i--) {
														c.wayp_pre[i] = c.wayp_pre[0];
													}
													c.wayp_pre[1] = ppp;
													c.wayp_pre[0] = pppa;
													c.ai_reload = false;
												}
												vec_2 = mapparts->get_waypoint()[c.wayp_pre[0]] - c.body.GetMatrix().pos();
												x_m = -int(vec_x.dot(vec_2) * 80);
												y_m = -int(vec_y.dot(vec_2) * 80);

												//到達時に判断
												if (vec_2.size() <= 0.5f) {
													int now = -1;
													auto poss = c.body.GetMatrix().pos();
													auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
													for (auto& w : mapparts->get_waypoint()) {
														auto id = &w - &mapparts->get_waypoint()[0];
														bool tt = true;
														for (auto& ww : c.wayp_pre) {
															if (id == ww) {
																tt = false;
															}
														}
														if (tt) {
															if (tmp.size() >= (w - poss).size()) {
																auto p = mapparts->map_col_line(w + VGet(0, 0.75f, 0), poss + VGet(0, 1.f, 0), 0);
																if (p.HitFlag == FALSE) {
																	tmp = (w - poss);
																	now = int(id);
																}
															}
														}
													}
													if (now != -1) {
														for (int i = int(c.wayp_pre.size()) - 1; i >= 1; i--) {
															c.wayp_pre[i] = c.wayp_pre[i - 1];
														}
														c.wayp_pre[0] = now;
													}
												}
											}
											break;
											default:
												break;
											}
										}

										c.mat *= MATRIX_ref::RotAxis(c.mat.zvec(), c.body_zrad).Inverse();
										if (c.qkey) {
											easing_set(&c.body_zrad, deg2rad(-30), 0.9f);
										}
										else if (c.ekey) {
											easing_set(&c.body_zrad, deg2rad(30), 0.9f);
										}
										else {
											easing_set(&c.body_zrad, 0.f, 0.9f);
										}
										c.mat *= MATRIX_ref::RotAxis(c.mat.zvec(), c.body_zrad);

										c.mat = MATRIX_ref::RotX(-c.xrad_p)*c.mat;
										c.xrad_p = std::clamp(c.xrad_p - deg2rad(std::clamp(y_m, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
										c.mat *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m, -120, 120))*0.1f);
										c.mat = MATRIX_ref::RotX(c.xrad_p)*c.mat;
									}
									//移動
									{
										auto speed = (c.running ? 6.f : ((c.ads.first ? 2.f : 4.f)*(c.squat.first ? 0.4f : 1.f))) / GetFPS();
										VECTOR_ref zv_t = c.mat.zvec();
										zv_t.y(0.f);
										zv_t = zv_t.Norm();

										VECTOR_ref xv_t = c.mat.xvec();
										xv_t.y(0.f);
										xv_t = xv_t.Norm();

										easing_set(&c.add_pos_buf,
											VECTOR_ref(c.wkey ? (zv_t*-speed) : VGet(0, 0, 0))
											+ (c.skey ? (zv_t*speed) : VGet(0, 0, 0))
											+ (c.akey ? (xv_t*speed) : VGet(0, 0, 0))
											+ (c.dkey ? (xv_t*-speed) : VGet(0, 0, 0))
											, 0.95f);

										if (c.add_ypos == 0.f) {
											if (false) {
												c.add_ypos = 0.06f;
											}
											c.add_pos = c.add_pos_buf;
										}
										else {
											easing_set(&c.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
									//操作
									{
										//マガジン取得
										c.down_mag = true;
										//引き金(左クリック)
										easing_set(&c.obj_gun.get_anime(2).per, float(shoot && !c.running), 0.5f);
										//ADS
										c.ads.first = aim && (!c.reloadf);
										//セレクター(中ボタン)
										c.selkey.get_in(false);
										//マグキャッチ(Rキー)
										easing_set(&c.obj_gun.get_anime(5).per, float(reload), 0.5f);
										//銃取得
										c.getmag.get_in(false);
									}
								}
							}
							//chara
							if (Drawparts->use_vr) {
								//HMD_mat
								{
									//+視点取得
									auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hmd_num()];
									mine.pos_HMD_old = mine.pos_HMD;
									Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &mine.pos_HMD, &mine.mat);
									if (mine.start_c || (ptr_.turn && ptr_.now) != oldv) {
										mine.rec_HMD = VGet(mine.pos_HMD.x(), 0.f, mine.pos_HMD.z());
									}
									oldv = ptr_.turn && ptr_.now;
								}
								//移動
								if (Drawparts->get_hand2_num() != -1) {
									auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
											//running
											mine.running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;
											auto speed = (mine.running ? 6.f : 4.f) / GetFPS();

											if (Drawparts->tracker_num.size() > 0) {
												auto p = mine.body.GetFrameLocalWorldMatrix(mine.frame_.bodyb_f.first);
												easing_set(&mine.add_pos_buf, (p.zvec()*-ptr_.touch.y() + p.xvec()*-ptr_.touch.x())*speed, 0.95f);
											}
											else {
												easing_set(&mine.add_pos_buf, (mine.mat.zvec()*ptr_.touch.y() + mine.mat.xvec()*ptr_.touch.x())*speed, 0.95f);
											}
										}
										else {
											easing_set(&mine.add_pos_buf, VGet(0, 0, 0), 0.95f);
										}
										if (mine.add_ypos == 0.f) {
											if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
												mine.add_ypos = 0.06f;
											}
											mine.add_pos = mine.add_pos_buf;
										}
										else {
											easing_set(&mine.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
								}
								//操作
								{
									if (Drawparts->get_hand1_num() != -1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
										if (ptr_.turn && ptr_.now) {
											//引き金
											easing_set(&mine.obj_gun.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f);
											//マグキャッチ
											easing_set(&mine.obj_gun.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f);
											//セレクター
											mine.selkey.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										}
									}
									if (Drawparts->get_hand2_num() != -1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
										if (ptr_.turn && ptr_.now) {
											//マガジン取得
											mine.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (mine.gun_stat[mine.gun_ptr->id].mag_in.size() >= 1));

											mine.getmag.get_in((ptr_.on[0] & BUTTON_TOPBUTTON_B) != 0);
										}
									}
								}
								//2P
								/*
								{
									auto& ct = chara[1];

									bool wkey = (CheckHitKey(KEY_INPUT_W) != 0);
									bool skey = (CheckHitKey(KEY_INPUT_S) != 0);
									bool akey = (CheckHitKey(KEY_INPUT_A) != 0);
									bool dkey = (CheckHitKey(KEY_INPUT_D) != 0);
									ct.running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
									ct.squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
									auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
									auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
									if (ct.running) {
										ct.squat.first = false;
									}
									if (ct.ads.first) {
										ct.running = false;
									}
									if (!wkey && !skey && !akey && !dkey) {
										ct.running = false;
									}
									if (ct.running) {
										skey = false;
										akey = false;
										dkey = false;
									}
									//HMD_mat
									{
										ct.mat *= MATRIX_ref::RotAxis(ct.mat.zvec(), ct.body_zrad).Inverse();
										if (qkey) {
											easing_set(&ct.body_zrad, deg2rad(-30), 0.9f);
										}
										else if (ekey) {
											easing_set(&ct.body_zrad, deg2rad(30), 0.9f);
										}
										else {
											easing_set(&ct.body_zrad, 0.f, 0.9f);
										}
										ct.mat *= MATRIX_ref::RotAxis(ct.mat.zvec(), ct.body_zrad);
										int32_t x_m, y_m;
										GetMousePoint(&x_m, &y_m);
										ct.mat = MATRIX_ref::RotX(-ct.xrad_p)*ct.mat;
										ct.xrad_p = std::clamp(ct.xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
										ct.mat *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
										ct.mat = MATRIX_ref::RotX(ct.xrad_p)*ct.mat;
										SetMousePoint(deskx / 2, desky / 2);
										SetMouseDispFlag(FALSE);
									}
									//移動
									{
										auto speed = (ct.running ? 6.f : ((ct.ads.first ? 2.f : 4.f)*(ct.squat.first ? 0.4f : 1.f))) / GetFPS();
										VECTOR_ref zv_t = ct.mat.zvec();
										zv_t.y(0.f);
										zv_t = zv_t.Norm();

										VECTOR_ref xv_t = ct.mat.xvec();
										xv_t.y(0.f);
										xv_t = xv_t.Norm();

										easing_set(&ct.add_pos_buf,
											VECTOR_ref(wkey ? (zv_t*-speed) : VGet(0, 0, 0))
											+ (skey ? (zv_t*speed) : VGet(0, 0, 0))
											+ (akey ? (xv_t*speed) : VGet(0, 0, 0))
											+ (dkey ? (xv_t*-speed) : VGet(0, 0, 0))
											, 0.95f);

										if (ct.add_ypos == 0.f) {
											if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
												ct.add_ypos = 0.06f;
											}
											ct.add_pos = ct.add_pos_buf;
										}
										else {
											easing_set(&ct.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
									//操作
									{
										//マガジン取得
										ct.down_mag = true;
										//引き金(左クリック)
										easing_set(&ct.obj_gun.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !ct.running), 0.5f);
										//ADS
										ct.ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (!ct.reloadf);
										//セレクター(中ボタン)
										ct.selkey.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
										//マグキャッチ(Rキー)
										easing_set(&ct.obj_gun.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0)), 0.5f);
										//銃取得
										ct.getmag.get_in(CheckHitKey(KEY_INPUT_F) != 0);
									}
								}
								//*/
							}
							else {
								bool wkey = (CheckHitKey(KEY_INPUT_W) != 0);
								bool skey = (CheckHitKey(KEY_INPUT_S) != 0);
								bool akey = (CheckHitKey(KEY_INPUT_A) != 0);
								bool dkey = (CheckHitKey(KEY_INPUT_D) != 0);
								mine.running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
								mine.squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
								mine.qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
								mine.ekey = (CheckHitKey(KEY_INPUT_E) != 0);
								if (mine.running) {
									mine.squat.first = false;
								}
								if (mine.ads.first) {
									mine.running = false;
								}
								if (!wkey && !skey && !akey && !dkey) {
									mine.running = false;
								}
								if (mine.running) {
									skey = false;
									akey = false;
									dkey = false;
								}

								//HMD_mat
								{
									mine.mat *= MATRIX_ref::RotAxis(mine.mat.zvec(), mine.body_zrad).Inverse();
									if (mine.qkey) {
										easing_set(&mine.body_zrad, deg2rad(-30), 0.9f);
									}
									else if (mine.ekey) {
										easing_set(&mine.body_zrad, deg2rad(30), 0.9f);
									}
									else {
										easing_set(&mine.body_zrad, 0.f, 0.9f);
									}
									mine.mat *= MATRIX_ref::RotAxis(mine.mat.zvec(), mine.body_zrad);
									int32_t x_m, y_m;
									GetMousePoint(&x_m, &y_m);
									mine.mat = MATRIX_ref::RotX(-mine.xrad_p)*mine.mat;
									mine.xrad_p = std::clamp(mine.xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
									mine.mat *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
									mine.mat = MATRIX_ref::RotX(mine.xrad_p)*mine.mat;
									SetMousePoint(deskx / 2, desky / 2);
									SetMouseDispFlag(FALSE);
								}
								//移動
								{
									auto speed = (mine.running ? 6.f : ((mine.ads.first ? 2.f : 4.f)*(mine.squat.first ? 0.4f : 1.f))) / GetFPS();
									VECTOR_ref zv_t = mine.mat.zvec();
									zv_t.y(0.f);
									zv_t = zv_t.Norm();

									VECTOR_ref xv_t = mine.mat.xvec();
									xv_t.y(0.f);
									xv_t = xv_t.Norm();

									easing_set(&mine.add_pos_buf,
										VECTOR_ref(wkey ? (zv_t*-speed) : VGet(0, 0, 0))
										+ (skey ? (zv_t*speed) : VGet(0, 0, 0))
										+ (akey ? (xv_t*speed) : VGet(0, 0, 0))
										+ (dkey ? (xv_t*-speed) : VGet(0, 0, 0))
										, 0.95f);
									if (mine.add_ypos == 0.f) {
										if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
											mine.add_ypos = 0.06f;
										}
										mine.add_pos = mine.add_pos_buf;
									}
									else {
										//easing_set(&mine.add_pos, VGet(0, 0, 0), 0.995f);
									}
								}
								//操作
								{
									//マガジン取得
									mine.down_mag = true;
									//引き金(左クリック)
									easing_set(&mine.obj_gun.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !mine.running), 0.5f);
									//ADS
									mine.ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (!mine.reloadf);
									//セレクター(中ボタン)
									mine.selkey.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
									//マグキャッチ(Rキー)
									easing_set(&mine.obj_gun.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0)), 0.5f);
									//銃取得
									mine.getmag.get_in(CheckHitKey(KEY_INPUT_F) != 0);
								}
							}
						}
						if (Drawparts->use_vr) {
							easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 3.f), 0.95f);
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
									{
										mapparts->map_col_wall(pos_t2, &pos_t);
									}
									c.pos = pos_t - (VECTOR_ref(VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
								}
								//共通
								{
									VECTOR_ref pos_t2 = (c.pos + (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
									VECTOR_ref pos_t = pos_t2 + c.add_pos;
									//壁
									{
										mapparts->map_col_wall(pos_t2, &pos_t);
										if ((c.add_pos - (pos_t - pos_t2)).size() != 0.f) {
											//c.add_pos = pos_t - pos_t2;
											if (c.add_ypos == 0.f) {
												//c.add_pos_buf = c.add_pos;
											}
										}
									}
									//落下
									{
										auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
										if (c.add_ypos <= 0.f && pp.HitFlag == 1) {
											if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(pp.Normal) >= cos(deg2rad(30))) {
												pos_t = pp.HitPosition;
											}
											else {
												//ブロックするベクトル
												auto v_t = VECTOR_ref(pp.Normal);
												v_t.y(0);
												v_t = v_t.Norm();
												//
												pos_t -= c.add_pos;
												c.add_pos += v_t * c.add_pos.cross(v_t.cross(c.add_pos).Norm()).size();
												if (c.add_ypos == 0.f) {
													c.add_pos_buf = c.add_pos;
												}
												pos_t += c.add_pos;
											}
											c.add_ypos = 0.f;
										}
										else {
											pos_t.yadd(c.add_ypos);
											c.add_ypos += M_GR / std::powf(GetFPS(), 2.f);
											//復帰
											if (pos_t.y() <= -5.f) {
												pos_t = c.spawn_pos;
												if (Drawparts->use_vr) {
													if (&c == &chara[1]) {
														c.xrad_p = 0;
													}
												}
												else {
													if (&c == &mine) {
														c.xrad_p = 0;
													}
												}
												c.spawn(pos_t, c.spawn_mat);
												{
													int now = -1;
													for (auto& w : c.wayp_pre) {
														w = now;
													}
													auto poss = c.pos + c.pos_HMD - c.rec_HMD;
													auto tmp = mapparts->get_waypoint()[0] - poss;
													for (auto& w : mapparts->get_waypoint()) {
														auto id = &w - &mapparts->get_waypoint()[0];
														bool tt = true;
														for (auto& ww : c.wayp_pre) {
															if (id == ww) {
																tt = false;
															}
														}
														if (tt) {
															if (tmp.size() >= (w - poss).size()) {
																auto p = mapparts->map_col_line(w + VGet(0, 1.f, 0), poss + VGet(0, 1.f, 0), 0);
																if (p.HitFlag == FALSE) {
																	tmp = (w - poss);
																	now = int(id);
																}
															}
														}
													}
													if(now!=-1){
														for (auto& w : c.wayp_pre) {
															w = now;
														}
													}
												}
											}
										}
									}
									//反映
									c.pos = pos_t - (VECTOR_ref(VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
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
									c.body_yrad += r_ * FRAME_RATE / GetFPS() / 10.f;
								}
								//身体,頭部,腰
								MATRIX_ref m_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								{
									c.body.SetMatrix(m_inv);
									if (Drawparts->tracker_num.size() > 0) {
										//腰
										c.body.SetFrameLocalMatrix(c.frame_.bodyb_f.first, (c.mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(c.frame_.bodyb_f.second));
										//頭部
										c.body.SetFrameLocalMatrix(c.frame_.head_f.first, (MATRIX_ref::Axis1(c.mat.xvec()*-1.f, c.mat.yvec(), c.mat.zvec()*-1.f) *m_inv.Inverse()*(c.mat_WAIST*m_inv.Inverse()).Inverse())
											*MATRIX_ref::Mtrans(c.frame_.head_f.second));
									}
									else {
										//頭部
										c.body.SetFrameLocalMatrix(c.frame_.head_f.first, (MATRIX_ref::Axis1(c.mat.xvec()*-1.f, c.mat.yvec(), c.mat.zvec()*-1.f) *m_inv.Inverse())
											*MATRIX_ref::Mtrans(c.frame_.head_f.second));
									}
									c.body.SetMatrix(m_inv *MATRIX_ref::Mtrans((c.pos + c.pos_HMD - c.rec_HMD) - (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f)));
								}
								//足
								{
									//左
									if (Drawparts->tracker_num.size() > 1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[1]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &mine.pos_LEFTREG, &mine.mat_LEFTREG);
										c.mat_LEFTREG = MATRIX_ref::Axis1(c.mat_LEFTREG.xvec()*-1.f, c.mat_LEFTREG.yvec(), c.mat_LEFTREG.zvec()*-1.f);
										if ((mine.start_c || (ptr_.turn && ptr_.now) != oldv_2) && oldv_2_2) {
										mine.mat_LEFTREG_rep = mine.mat_LEFTREG;
										if (!mine.start_c) {
											//oldv_2_2 = false;
										}
										}
										oldv_2 = ptr_.turn && ptr_.now;
										mine.mat_LEFTREG = 
											MATRIX_ref::RotY(deg2rad(90+60-10))*
											mine.mat_LEFTREG_rep.Inverse()*mine.mat_LEFTREG;
										mine.pos_LEFTREG = mine.pos_LEFTREG + (mine.pos - mine.rec_HMD);
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_LEFTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.LEFTreg_f.first, c.mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTreg_f.second));
										}

										{
											auto pp = mapparts->map_col_line(mine.body.frame(mine.frame_.LEFTreg2_f.first) + VGet(0, 1.6f, 0), mine.body.frame(mine.frame_.LEFTreg2_f.first), 0);
											if (pp.HitFlag) {
												mine.pos_LEFTREG = VECTOR_ref(pp.HitPosition) - (mine.body.frame(mine.frame_.LEFTreg2_f.first) - mine.body.frame(mine.frame_.LEFTreg_f.first));
											}
										}

										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_LEFTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.LEFTreg_f.first, c.mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTreg_f.second));
										}
									}
									//右
									if (Drawparts->tracker_num.size() > 2) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[2]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &mine.pos_RIGHTREG, &mine.mat_RIGHTREG);
										c.mat_RIGHTREG = MATRIX_ref::Axis1(c.mat_RIGHTREG.xvec()*-1.f, c.mat_RIGHTREG.yvec(), c.mat_RIGHTREG.zvec()*-1.f);
										if ((mine.start_c || (ptr_.turn && ptr_.now) != oldv_3) && oldv_3_2) {
											mine.mat_RIGHTREG_rep = mine.mat_RIGHTREG;
											if (!mine.start_c) {
												//oldv_3_2 = false;
											}
										}
										oldv_3 = ptr_.turn && ptr_.now;
										mine.mat_RIGHTREG = 
											MATRIX_ref::RotY(deg2rad(180-22-10))*
											mine.mat_RIGHTREG_rep.Inverse()*mine.mat_RIGHTREG;
										mine.pos_RIGHTREG = mine.pos_RIGHTREG + (mine.pos - mine.rec_HMD);
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_RIGHTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_RIGHTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする


											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTreg_f.first, c.mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTreg_f.second));
										}

										{
											auto pp = mapparts->map_col_line(mine.body.frame(mine.frame_.RIGHTreg2_f.first) + VGet(0, 1.6f, 0), mine.body.frame(mine.frame_.RIGHTreg2_f.first), 0);
											if (pp.HitFlag) {
												mine.pos_RIGHTREG = VECTOR_ref(pp.HitPosition) - (mine.body.frame(mine.frame_.RIGHTreg2_f.first) - mine.body.frame(mine.frame_.RIGHTreg_f.first));
											}
										}
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_RIGHTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
											//VECTOR_ref vec_a1L1 = (mine.mat_RIGHTREG*mine.mat.Inverse()).zvec()*-1.f;//x=0とする

											VECTOR_ref vec_a1L1 = VGet(0, 0, -1.f);

											float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTreg_f.first, c.mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTreg_f.second));
										}
									}

								}
								//手
								{
									c.body.get_anime(3).per = 0.f;
									c.body.get_anime(3).time = 0.f;
									//右手
									{
										Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &c.pos_RIGHTHAND, &c.mat_RIGHTHAND);
										c.pos_RIGHTHAND = c.pos_RIGHTHAND + (c.pos - c.rec_HMD);
										c.mat_RIGHTHAND = MATRIX_ref::Axis1(c.mat_RIGHTHAND.xvec()*-1.f, c.mat_RIGHTHAND.yvec(), c.mat_RIGHTHAND.zvec()*-1.f);
										c.mat_RIGHTHAND = c.mat_RIGHTHAND*MATRIX_ref::RotAxis(c.mat_RIGHTHAND.xvec(), deg2rad(-60));
										c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_RIGHTHAND;//リコイル
										//銃器
										c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
										{
											//基準
											VECTOR_ref tgt_pt = c.obj_gun.frame(c.gun_ptr->frame[5].first);
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//基準
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
											float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.RIGHThand_f.first,
												MATRIX_ref::RotY(deg2rad(-10))*
												MATRIX_ref::RotZ(deg2rad(50))*
												MATRIX_ref::RotX(deg2rad(90))*
												c.mat_RIGHTHAND*
												m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHThand_f.second));
										}
										//右人差し指
										c.body.get_anime(0).per = 1.f;
										c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
									}
									//左手
									{
										Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &c.pos_LEFTHAND, &c.mat_LEFTHAND);
										c.pos_LEFTHAND = c.pos_LEFTHAND + (c.pos - c.rec_HMD);
										c.mat_LEFTHAND = MATRIX_ref::Axis1(c.mat_LEFTHAND.xvec()*-1.f, c.mat_LEFTHAND.yvec(), c.mat_LEFTHAND.zvec()*-1.f);
										c.mat_LEFTHAND = c.mat_LEFTHAND*MATRIX_ref::RotAxis(c.mat_LEFTHAND.xvec(), deg2rad(-60));

										{
											float dist_ = (c.pos_LEFTHAND - c.obj_gun.frame(c.gun_ptr->frame[3].first)).size();
											if (dist_ <= 0.1f && (!c.reloadf || !c.down_mag)) {
												c.LEFT_hand = true;
												c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame[3].first);
											}
											else {
												c.LEFT_hand = false;
											}
										}
										{
											//基準
											VECTOR_ref tgt_pt = c.pos_LEFTHAND;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
											float cos_t = getcos_tri((c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_.LEFTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//上腕
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}

											//下腕
											c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
											//手
											c.body.SetFrameLocalMatrix(c.frame_.LEFThand_f.first, MATRIX_ref::RotZ(deg2rad(-60))* MATRIX_ref::RotX(deg2rad(80))* c.mat_LEFTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFThand_f.second));
										}

									}
								}
							}
							else {
								{
									VECTOR_ref v_ = c.mat.zvec();
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * FRAME_RATE / GetFPS() / 10.f;
								}
								{
									VECTOR_ref v_ = c.mat.zvec();
									float x_1 = sinf(c.body_xrad);
									float y_1 = -cosf(c.body_xrad);
									float x_2 = -v_.y();
									float y_2 = -std::hypotf(v_.x(), v_.z());
									c.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
								}
								//身体
								MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15));
								MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*mg_inv;
								{
									if (c.reloadf) {
										mb_inv = MGetIdent();
										m_inv = MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*mg_inv;
									}
									c.body.SetMatrix(MATRIX_ref::Mtrans(c.pos - c.rec_HMD));
									//
									c.body.SetFrameLocalMatrix(c.frame_.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(c.frame_.bodyg_f.second));
									c.body.SetFrameLocalMatrix(c.frame_.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(c.frame_.bodyb_f.second));
									c.body.SetFrameLocalMatrix(c.frame_.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(c.frame_.body_f.second));
								}

								//頭部
								c.body.SetFrameLocalMatrix(c.frame_.head_f.first, c.mat*m_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.head_f.second));
								if (c.reloadf) {
									c.body.frame_reset(c.frame_.head_f.first);
								}
								//足
								{
									auto speed = (c.running ? 6.f : ((c.ads.first ? 2.f : 4.f)*(c.squat.first ? 0.4f : 1.f))) / GetFPS();
									auto ratio_t = c.add_pos.size() / speed;
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
									c.body.get_anime(1).time += 30.f / GetFPS();
									if (c.body.get_anime(1).time >= c.body.get_anime(1).alltime) {
										c.body.get_anime(1).time = 0.f;
									}
									c.body.get_anime(2).time += 30.f / GetFPS();
									if (c.body.get_anime(2).time >= c.body.get_anime(2).alltime) {
										c.body.get_anime(2).time = 0.f;
									}
									c.body.get_anime(8).time += 30.f / GetFPS() * ((c.body.get_anime(8).alltime / 30.f) / c.gun_ptr->reload_time);
									if (c.body.get_anime(8).time >= c.body.get_anime(8).alltime) {
										c.body.get_anime(8).time = 0.f;
									}
								}
								//視点
								{
									VECTOR_ref pv = VGet(0, 0, 0);
									if (c.gun_ptr->frame[4].first != INT_MAX) {
										pv = c.gun_ptr->frame[4].second;
									}
									if (c.ads.first) {
										easing_set(&c.gunpos, VGet(-0.f, 0.f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.001f*c.body.get_anime(1).per, -0.315f), 0.75f);
										if (&c == &mine) {
											easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.f), 0.9f);
										}
									}
									else {
										if (c.running) {
											easing_set(&c.gunpos, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f);
											if (&c == &mine) {
												easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 3.f), 0.95f);
											}
										}
										else {
											easing_set(&c.gunpos, VGet(-0.1f, -0.05f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.002f*c.body.get_anime(1).per, -0.335f), 0.75f);
											if (&c == &mine) {
												easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.75f), 0.95f);
											}
										}
									}
								}
								//手
								{
									c.body.frame_reset(c.frame_.RIGHTarm1_f.first);
									c.body.frame_reset(c.frame_.RIGHTarm2_f.first);
									c.body.frame_reset(c.frame_.RIGHThand_f.first);
									c.body.frame_reset(c.frame_.LEFTarm1_f.first);
									c.body.frame_reset(c.frame_.LEFTarm2_f.first);
									c.body.frame_reset(c.frame_.LEFThand_f.first);


									if (c.running) {
										if (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
											c.body.get_anime(3).per = 1.f;
											c.body.get_anime(3).time += 30.f / GetFPS() * ((c.body.get_anime(3).alltime / 30.f) / c.gun_ptr->reload_time);
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
											c.body.get_anime(6).time += 30.f / GetFPS();
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
											c.body.get_anime(3).time += 30.f / GetFPS() * ((c.body.get_anime(3).alltime / 30.f) / c.gun_ptr->reload_time);
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
												c.pos_HMD = (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f) -c.pos;
												//銃器
												c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat;//リコイル
												c.pos_RIGHTHAND = (c.pos + c.pos_HMD - c.rec_HMD) + (MATRIX_ref::Vtrans(c.gunpos, c.mat_RIGHTHAND) - c.rec_HMD);
												c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
												//基準
												VECTOR_ref tgt_pt = c.obj_gun.frame(c.gun_ptr->frame[5].first);
												VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
												VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
												float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm1_f.first) - tgt_pt).size());
												VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
												//上腕
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
												MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
												//下腕
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
												MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
												//手
												c.body.SetFrameLocalMatrix(c.frame_.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* c.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHThand_f.second));
											}
											//左手
											{
												if (c.down_mag) {
													c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame[0].first) + c.mat_RIGHTHAND.yvec()*-0.05f;
												}
												else {
													c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame[3].first);
												}
												if (!c.reloadf || !c.down_mag) {
													c.LEFT_hand = true;
													c.pos_LEFTHAND = c.obj_gun.frame(c.gun_ptr->frame[3].first);
												}
												else {
													c.LEFT_hand = false;
												}
												c.mat_LEFTHAND = c.mat;

												{
													VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((c.pos_LEFTHAND - c.body.frame(c.frame_.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
													VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
													float cos_t = getcos_tri((c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_.LEFTarm1_f.first) - (c.pos + c.pos_LEFTHAND - (c.pos - c.rec_HMD))).size());
													VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
													//上腕
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
													MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first), m_inv.Inverse()),
														vec_t
													);
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
													//下腕
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
													MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
														MATRIX_ref::Vtrans(c.pos_LEFTHAND - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
													);
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
													//手
													c.body.SetFrameLocalMatrix(c.frame_.LEFThand_f.first,
														MATRIX_ref::RotZ(deg2rad(-60))*
														MATRIX_ref::RotX(deg2rad(80))*
														c.mat_LEFTHAND*
														m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFThand_f.second));
												}
											}
										}
									}
									//右人差し指
									{
										c.body.get_anime(0).per = 1.f;
										c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
									}
								}
							}
							c.body.work_anime();

							//

							c.frame_.copy_frame(c.body, c.colframe_, &c.col);
							c.col.work_anime();
							c.col.RefreshCollInfo(-1, 0);
							c.col.RefreshCollInfo(-1, 1);
							c.col.RefreshCollInfo(-1, 2);
							//
							if (!(Drawparts->use_vr && (&c == &mine))) {
								//視点取得
								c.pos_HMD = (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f) - c.pos;
								//銃器
								if (c.running || (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1)) {
									auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.frame_.RIGHThand2_f.first);
									c.pos_RIGHTHAND = c.body.frame(c.frame_.RIGHThand_f.first);
									c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
									c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND - c.obj_gun.frame(c.gun_ptr->frame[5].first) + c.pos_RIGHTHAND));
									//
									c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.frame_.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.frame_.LEFThand2_f.first)).Inverse());
									c.pos_LEFTHAND = c.body.frame(c.frame_.LEFThand_f.first) + c.mat_LEFTHAND.yvec()*0.1f;
								}
								else {
									auto mat_T = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat;//リコイル
									c.pos_RIGHTHAND = c.pos + c.pos_HMD + MATRIX_ref::Vtrans(c.gunpos, mat_T);
									c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
								}
							}
							//銃共通
							{
								if (c.obj_gun.get_anime(3).per == 1.f) {
									c.audio.slide.play_3D(c.pos_RIGHTHAND, 15.f);
								}
								c.obj_gun.get_anime(3).per = std::max(c.obj_gun.get_anime(3).per - 12.f / GetFPS(), 0.f);
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
											c.obj_gun.get_anime(0).time += 60.f / GetFPS();
											if (c.obj_gun.get_anime(0).time >= c.obj_gun.get_anime(0).alltime) {
												c.obj_gun.get_anime(0).time = 0.f;
												c.gunf = false;
											}
										}
										else {
											c.obj_gun.get_anime(1).per = 1.f;
											c.obj_gun.get_anime(0).per = 0.f;
											c.obj_gun.get_anime(1).time += 60.f / GetFPS();
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
										for (auto& g : this->item_data) {
											if (g.ptr == nullptr && g.cate == 1) {
												tt = true;
												g.Set_item(c.gun_ptr, c.pos_mag, c.mat_mag, 1);
												g.add = (c.obj_gun.frame(c.gun_ptr->frame[1].first) - c.obj_gun.frame(c.gun_ptr->frame[0].first)).Norm()*-1.f / GetFPS();//排莢ベクトル
												g.magazine.cap = dnm;
												g.del_timer = 0.f;
												break;
											}
										}
										if (!tt) {
											this->item_data.resize(this->item_data.size() + 1);
											auto& g = this->item_data.back();
											g.Set_item(c.gun_ptr, c.pos_mag, c.mat_mag, 1);
											g.add = (c.obj_gun.frame(c.gun_ptr->frame[1].first) - c.obj_gun.frame(c.gun_ptr->frame[0].first)).Norm()*-1.f / GetFPS();//排莢ベクトル
											g.magazine.cap = dnm;
											g.del_timer = 0.f;
										}
										//
										c.reloadf = true;
									}
									//マガジン挿入
									if (c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1) {
										if (Drawparts->use_vr) {
											if (c.reload_cnt < c.gun_ptr->reload_time) {
												c.down_mag = false;
											}
											if (c.down_mag) {
												if ((c.obj_mag.frame(3) - c.obj_gun.frame(c.gun_ptr->frame[0].first)).size() <= 0.05f) {
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
												c.mat_mag = c.obj_mag.GetFrameLocalMatrix(3)* (c.mat_LEFTHAND*MATRIX_ref::RotVec2(c.mat_LEFTHAND.yvec(), (c.obj_gun.frame(c.gun_ptr->frame[0].first) - c.pos_LEFTHAND)));
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
										c.reload_cnt += 1.f / GetFPS();//挿入までのカウント
										//
									}
									else {
										c.down_mag = false;
										c.pos_mag = c.obj_gun.frame(c.gun_ptr->frame[1].first);
										c.mat_mag = c.mat_RIGHTHAND;
									}
									//セレクター
									if (c.selkey.second == 1) {
										++c.gun_stat[c.gun_ptr->id].select %= c.gun_ptr->select.size();
									}
									//セフティ
									easing_set(&c.obj_gun.get_anime(4).per, float(0.f), 0.5f);
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
											c.bullet[c.use_bullet].set(&c.gun_ptr->ammo[0], c.obj_gun.frame(c.gun_ptr->frame[2].first), c.mat_RIGHTHAND.zvec()*-1.f);
											++c.use_bullet %= c.bullet.size();//次のIDへ
											//エフェクト
											c.effcs[ef_fire].set(c.obj_gun.frame(c.gun_ptr->frame[2].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);
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
								c.canget_magitem = false;
								if (&c == &chara[0]) {
									for (auto& g : this->item_data) {
										g.Get_item_1(item_data, mapparts);
									}
								}
								for (auto& g : this->item_data) {
									g.Get_item_2(c, mapparts);
								}

								for (auto& g : this->item_data) {
									if (g.ptr != nullptr && g.cate == 1 && g.magazine.cap==0) {
										g.del_timer += 1.f / GetFPS();
									}
								}
								while (true) {
									bool p = false;
									size_t id = 0;
									for (auto& g : this->item_data) {
										if (g.ptr!=nullptr && g.cate == 1) {
											if (g.del_timer >= 5.f) {
												g.Delete_item();
												id = &g - &this->item_data[0];
												p = true;
											}
										}
									}
									if (p) {
										this->item_data.erase(this->item_data.begin() + id);
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
									c_.body.PhysicsCalculation(1000.f / GetFPS());
								}
							}
						}
						//campos,camvec,camupの指定
						{
							//auto& cc = chara[1];
							auto& cc = mine;
							if (Drawparts->use_vr) {
								cam_easy.campos = cc.pos + cc.pos_HMD - cc.rec_HMD;
								cam_easy.camvec = cam_easy.campos + cc.mat.zvec();
								cam_easy.camup = cc.mat.yvec();
							}
							else {
								cam_easy.campos = cc.pos + cc.pos_HMD - cc.rec_HMD;
								cam_easy.camvec = cam_easy.campos + cc.mat.zvec()*-1.f;
								cam_easy.camup = cc.mat.yvec();
							}
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(cam_easy.campos.get(), cam_easy.camvec.get(), cam_easy.camup.get());
						UpdateEffekseer3D();
						//VR空間に適用
						Drawparts->Move_Player();
						//hit時に何かする部分
						for (auto& a : mine.bullet) {
							if (a.hit) {
								a.hit = false;
							}
						}
						//1P描画
						{
							//影用意
							Drawparts->Ready_Shadow(cam_easy.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
								for (auto& g : this->item_data) { g.Draw_item(); }
							},
								VGet(5.f, 2.5f, 5.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//書き込み
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(mine, Drawparts->use_vr);
								}
							}
							//VRに移す
							Drawparts->draw_VR( [&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = cam_easy;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									Hostpassparts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//最終描画
									Hostpassparts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);

								//main
								Hostpassparts->get_main().DrawGraph(0, 0, true);

								SetCameraNearFar(0.01f, 2.f);
								SetUseZBuffer3D(FALSE);												//zbufuse
								SetWriteZBuffer3D(FALSE);											//zbufwrite
								{
									//main
									//DrawBillboard3D((cam_easy.campos + (cam_easy.camvec - cam_easy.campos).Norm()*1.0f).get(), 0.5f, 0.5f, Drawparts->use_vr ? 1.8f : 1.475f, 0.f, Hostpassparts->get_main().get(), TRUE);
									//UI
									DrawBillboard3D((cam_easy.campos + (cam_easy.camvec - cam_easy.campos).Norm()*1.0f).get(), 0.5f, 0.5f, Drawparts->use_vr ? 1.8f : 1.475f, 0.f, this->UI_Screen.get(), TRUE);
								}
								SetUseZBuffer3D(TRUE);												//zbufuse
								SetWriteZBuffer3D(TRUE);											//zbufwrite
								//UI2
								UIparts->item_draw(chara, this->item_data, cam_easy.campos, Drawparts->use_vr);
							}, cam_easy);
						}
						if (!this->TPS.first) {
							//2P描画
							if (Drawparts->use_vr) {
								auto& ct = chara[1];
								//cam_s.cam
								cam_easy2.campos = ct.pos + ct.pos_HMD - ct.rec_HMD;
								cam_easy2.camvec = cam_easy2.campos + ct.mat.zvec()*-1.f;
								cam_easy2.camup = ct.mat.yvec();

								//影用意
								Drawparts->Ready_Shadow(cam_easy2.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
									for (auto& g : this->item_data) { g.Draw_item(); }
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
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, cam_easy2);
									//最終描画
									Hostpass2parts->MAIN_draw();
								}
								//Screen2に移す
								outScreen2.SetDraw_Screen(cam_easy2.campos, cam_easy2.camvec, cam_easy2.camup, cam_easy2.fov, cam_easy2.near_, cam_easy2.far_);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
									//UI
									this->UI_Screen2.DrawGraph(0, 0, true);
									//UI2
									UIparts->item_draw(chara, this->item_data, cam_easy2.campos, false);
								}
							}
						}
						//ディスプレイ描画
						{
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							//TPS視点
							if (this->TPS.first) {
								{
									if (CheckHitKey(KEY_INPUT_4) != 0) {
										sel_cam = 3;
									}
									if (CheckHitKey(KEY_INPUT_3) != 0) {
										sel_cam = 2;
									}
									if (CheckHitKey(KEY_INPUT_2) != 0) {
										sel_cam = 1;
									}
									if (CheckHitKey(KEY_INPUT_1) != 0) {
										sel_cam = 0;
									}

									VECTOR_ref vec_2 = chara[sel_cam].body.frame(chara[sel_cam].frame_.head_f.first) - cam_easy3.campos;
									VECTOR_ref vec_y = cam_easy3.camup;
									VECTOR_ref vec_x = (cam_easy3.camvec - cam_easy3.campos).cross(vec_y);

									{
										//xr_cam -= deg2rad(int(vec_y.dot(vec_2) * 5))*0.1f;
										yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 5))*0.1f;
									}

									if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
										yr_cam -= deg2rad(60) / GetFPS();
									}
									if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
										yr_cam += deg2rad(60) / GetFPS();
									}
									if (CheckHitKey(KEY_INPUT_UP) != 0) {
										xr_cam -= deg2rad(60) / GetFPS();
									}
									if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
										xr_cam += deg2rad(60) / GetFPS();
									}

									if (CheckHitKey(KEY_INPUT_A) != 0) {
										cam_easy3.campos.x(cam_easy3.campos.x() - 10.f / GetFPS()*cos(yr_cam));
										cam_easy3.campos.z(cam_easy3.campos.z() + 10.f / GetFPS()*sin(yr_cam));
									}
									if (CheckHitKey(KEY_INPUT_D) != 0) {
										cam_easy3.campos.x(cam_easy3.campos.x() + 10.f / GetFPS()*cos(yr_cam));
										cam_easy3.campos.z(cam_easy3.campos.z() - 10.f / GetFPS()*sin(yr_cam));
									}
									if (CheckHitKey(KEY_INPUT_W) != 0) {
										cam_easy3.campos.z(cam_easy3.campos.z() + 10.f / GetFPS()*cos(yr_cam));
										cam_easy3.campos.x(cam_easy3.campos.x() + 10.f / GetFPS()*sin(yr_cam));
									}
									if (CheckHitKey(KEY_INPUT_S) != 0) {
										cam_easy3.campos.z(cam_easy3.campos.z() - 10.f / GetFPS()*cos(yr_cam));
										cam_easy3.campos.x(cam_easy3.campos.x() - 10.f / GetFPS()*sin(yr_cam));
									}
									cam_easy3.campos.x(std::clamp(cam_easy3.campos.x(), -10.f, 10.f));
									cam_easy3.campos.z(std::clamp(cam_easy3.campos.z(), -10.f, 10.f));
									//xr_cam = std::clamp(xr_cam, deg2rad(-89), deg2rad(89));

									cam_easy3.camvec = cam_easy3.campos + MATRIX_ref::Vtrans(VGet(0, 0, 1), MATRIX_ref::RotX(xr_cam)*MATRIX_ref::RotY(yr_cam));
									cam_easy3.camup = VGet(0, 1.f, 0);
								}
								//影用意
								Drawparts->Ready_Shadow(cam_easy3.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
									for (auto& g : this->item_data) { g.Draw_item(); }
								},
									VGet(5.f, 2.5f, 5.f),
									VGet(5.f, 2.5f, 5.f)
									);
								//書き込み
								{
									//被写体深度描画
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, cam_easy3);
									//最終描画
									Hostpass2parts->MAIN_draw();
								}
								UI_minimap.SetDraw_Screen(true);
								{
									int xp = 0;
									int yp = 0;
									float radp = 0.f;

									xp = 150;
									yp = 150;
									{
										auto& cc = mine;
										auto t = cc.body.GetMatrix().pos();
										VECTOR_ref vec_z = cc.body.GetFrameLocalWorldMatrix(cc.frame_.head_f.first).zvec()*-1.f;
										radp = -atan2f(vec_z.x(), vec_z.z());
										auto x_2 = t.x() / 10.f *(mapparts->get_x_size() / 2);
										auto y_2 = -t.z() / 10.f *(mapparts->get_y_size() / 2);
										xp -= int(x_2*cos(radp) - y_2 * sin(radp));
										yp -= int(y_2*cos(radp) + x_2 * sin(radp));
									}

									DrawRotaGraph(xp, yp, 1.f, radp, mapparts->get_minmap().get(), TRUE);
									for (auto& c : this->chara) {
										auto t = c.body.GetMatrix().pos();
										VECTOR_ref vec_z = c.body.GetFrameLocalWorldMatrix(c.frame_.head_f.first).zvec()*-1.f;
										auto rad = atan2f(vec_z.x(), vec_z.z());
										auto x_2 = t.x() / 10.f *(mapparts->get_x_size() / 2);
										auto y_2 = -t.z() / 10.f *(mapparts->get_y_size() / 2);

										int xt = xp + int(x_2*cos(radp) - y_2 * sin(radp));
										int yt = yp + int(y_2*cos(radp) + x_2 * sin(radp));

										DrawRotaGraph(xt, yt, 1.f, rad + radp, UI_player.get(), TRUE);
									}
									{
										auto& cc = mine;
										auto t = cc.body.GetMatrix().pos();
										VECTOR_ref vec_z = cc.body.GetFrameLocalWorldMatrix(cc.frame_.head_f.first).zvec()*-1.f;
										auto x_2 = t.x() / 10.f *(mapparts->get_x_size() / 2);
										auto y_2 = -t.z() / 10.f *(mapparts->get_y_size() / 2);

										int xt = xp + int(x_2*cos(radp) - y_2 * sin(radp));
										int yt = yp + int(y_2*cos(radp) + x_2 * sin(radp));

										DrawCircle(xt, yt, 150, GetColor(0, 0, 0), FALSE, 3);
									}
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
									UI_minimap.DrawGraph(64, 64, true);
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
								}
							}
							//デバッグ
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
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