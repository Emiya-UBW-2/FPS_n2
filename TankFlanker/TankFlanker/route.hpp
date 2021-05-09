#pragma once
enum scenes
{
	ITEM_LOAD,
	MAP_LOAD,
	LOAD,
	SELECT,
	MAIN_LOOP
};

class main_c {
	//終了処理フラグ
	bool ending = true;
	int sel_scene = scenes::ITEM_LOAD;
	GraphHandle UI_Screen;
	cam_info* cam_t = nullptr;
public:
	main_c(void) {
		auto OPTPTs = std::make_unique<OPTION>();																						//設定読み込み
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);					//汎用
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);																			//デバッグ
		if (DrawPts->use_vr) {
			OPTPTs->DoF = false;
			OPTPTs->Bloom = false;
			OPTPTs->SSAO = false;//SSAOはVRでは使えない
		}
		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//ホストパスエフェクト(VR、フルスクリーン共用)
		auto MAPPTs = std::make_unique<MAPclass::Map>();																				//MAP
		auto UI_LOADPTs = std::make_unique<UIclass::UI_Load>();																			//UI_Load
		UI_Screen = GraphHandle::Make(DrawPts->disp_x, DrawPts->disp_y, true);															//VR、フルスクリーン共用
		//キー読み込み
		key_bind k_;
		k_.load_keyg();
		//シーン
		auto LOADscene = std::make_unique<Sceneclass::LOAD>(DrawPts);
		auto SELECTscene = std::make_unique<Sceneclass::SELECT>(DrawPts);
		auto MAINLOOPscene = std::make_unique<Sceneclass::MAINLOOP>(DrawPts, OPTPTs);
		//繰り返し
		do {
			bool selend = true;
			//開始
			{
				//常時表示のリセット
				k_.reSet_isalways();
				//
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					//アイテム読み込み
					UI_LOADPTs->Set("アイテムデータ");
					break;
				case scenes::MAP_LOAD:
					//マップ読み込み
					MAPPTs->Ready_map("data/map_new2");
					UI_LOADPTs->Set("マップ");
					break;
				case scenes::LOAD:
					//キャラ設定
					MAINLOOPscene->Set_Charaa(MAPPTs->get_spawn_point().size());
					LOADscene->Set(MAINLOOPscene);
					break;
				case scenes::SELECT:
					//
					SELECTscene->Set(OPTPTs, MAINLOOPscene);
					DrawPts->Set_Light_Shadow(SELECTscene->get_Shadow_maxpos(), SELECTscene->get_Shadow_minpos(), SELECTscene->get_Light_vec(), [&] {SELECTscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(SELECTscene->get_Light_color());
					break;
				case scenes::MAIN_LOOP:
					MAINLOOPscene->Set(OPTPTs, MAPPTs);
					DrawPts->Set_Light_Shadow(MAINLOOPscene->get_Shadow_maxpos(), MAINLOOPscene->get_Shadow_minpos(), MAINLOOPscene->get_Light_vec(), [&] {MAINLOOPscene->Shadow_Draw_Far(MAPPTs); });
					SetGlobalAmbientLight(MAINLOOPscene->get_Light_color());
					break;
				default:
					break;
				}
			}
			//メイン
			while (ProcessMessage() == 0) {
				const auto waits = GetNowHiPerformanceCount();
				{
					DebugPTs->put_way();
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						//MAINLOOPscene->get_mine().get_key_().Set(false, &k_);
						selend = UI_LOADPTs->Update();
						cam_t = &SELECTscene->get_camera();
						break;
					case scenes::MAP_LOAD:
						//MAINLOOPscene->get_mine().get_key_().Set(false, &k_);
						selend = UI_LOADPTs->Update();
						cam_t = &SELECTscene->get_camera();
						break;
					case scenes::LOAD:
						MAINLOOPscene->get_mine().get_key_().Set(false, &k_);
						selend = LOADscene->UpDate(k_);
						cam_t = &SELECTscene->get_camera();
						break;
					case scenes::SELECT:
						//
						if (DrawPts->use_vr) {
							MAINLOOPscene->get_mine().get_key_().Set_VR(DrawPts);
						}
						else {
							MAINLOOPscene->get_mine().get_key_().Set(false, &k_);
						}
						selend = SELECTscene->UpDate(MAPPTs, DrawPts, MAINLOOPscene, k_);
						cam_t = &SELECTscene->get_camera();
						break;
					case scenes::MAIN_LOOP:
						//
						if (DrawPts->use_vr) {
							MAINLOOPscene->get_mine().get_key_().Set_VR(DrawPts);
						}
						else {
							MAINLOOPscene->get_mine().get_key_().Set(false, &k_);
						}
						selend = MAINLOOPscene->UpDate(MAPPTs, DrawPts, k_);
						cam_t = &MAINLOOPscene->get_camera();
						break;
					default:
						break;
					}
					Set3DSoundListenerPosAndFrontPosAndUpVec(cam_t->campos.get(), cam_t->camvec.get(), cam_t->camup.get());
					//VR空間に適用
					DrawPts->Move_Player();
					//描画
					{
						//影用意
						DrawPts->Ready_Shadow(cam_t->campos, [&] {
							switch (sel_scene) {
							case scenes::ITEM_LOAD:
								break;
							case scenes::MAP_LOAD:
								break;
							case scenes::LOAD:
								LOADscene->Shadow_Draw();
								break;
							case scenes::SELECT:
								SELECTscene->Shadow_Draw();
								break;
							case scenes::MAIN_LOOP:
								MAINLOOPscene->Shadow_Draw();
								break;
							default:
								break;
							}

						}, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
						//↑nearはこれ
						//	(MAINLOOPscene->get_mine().get_alive()) ? VGet(2.f, 2.5f, 2.f) : VGet(10.f, 2.5f, 10.f)
						//書き込み
						this->UI_Screen.SetDraw_Screen();
						{
							switch (sel_scene) {
							case scenes::ITEM_LOAD:
								UI_LOADPTs->UI_Draw();
								break;
							case scenes::MAP_LOAD:
								UI_LOADPTs->UI_Draw();
								break;
							case scenes::LOAD:
								LOADscene->UI_Draw(MAINLOOPscene);
								break;
							case scenes::SELECT:
								SELECTscene->UI_Draw(MAINLOOPscene);
								break;
							case scenes::MAIN_LOOP:
								MAINLOOPscene->UI_Draw();
								break;
							default:
								break;
							}
						}
						//VRに移す
						DrawPts->draw_VR([&] {
							auto tmp = GetDrawScreen();
							auto tmp_cam = *cam_t;
							tmp_cam.campos = GetCameraPosition();
							tmp_cam.camvec = GetCameraTarget();
							{
								//被写体深度描画
								switch (sel_scene) {
								case scenes::ITEM_LOAD:
									break;
								case scenes::MAP_LOAD:
									break;
								case scenes::LOAD:
									HostpassPTs->BUF_draw([&](void) { LOADscene->BG_Draw(); }, [&](void) { DrawPts->Draw_by_Shadow([&] { LOADscene->Main_Draw(); }); }, tmp_cam);
									break;
								case scenes::SELECT:
									HostpassPTs->BUF_draw([&](void) { SELECTscene->BG_Draw(); }, [&](void) { DrawPts->Draw_by_Shadow([&] { SELECTscene->Main_Draw(); }); }, tmp_cam);
									break;
								case scenes::MAIN_LOOP:
									HostpassPTs->BUF_draw([&](void) { MAINLOOPscene->BG_Draw(MAPPTs); }, [&](void) { DrawPts->Draw_by_Shadow([&] { MAINLOOPscene->Main_Draw(MAPPTs); }); }, tmp_cam);
									break;
								default:
									break;
								}
								//最終描画
								HostpassPTs->MAIN_draw();
							}
							GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
							{
								HostpassPTs->get_main().DrawGraph(0, 0, true);
								//UI
								if (DrawPts->use_vr) {
									SetCameraNearFar(0.01f, 2.f);
									SetUseZBuffer3D(FALSE);												//zbufuse
									SetWriteZBuffer3D(FALSE);											//zbufwrite
									{
										DrawBillboard3D((cam_t->campos + (cam_t->camvec - cam_t->campos).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, this->UI_Screen.get(), TRUE);
									}
									SetUseZBuffer3D(TRUE);												//zbufuse
									SetWriteZBuffer3D(TRUE);											//zbufwrite
								}
								else {
									this->UI_Screen.DrawGraph(0, 0, TRUE);
								}
								//UI2
								switch (sel_scene) {
								case scenes::ITEM_LOAD:
									break;
								case scenes::MAP_LOAD:
									break;
								case scenes::LOAD:
									break;
								case scenes::SELECT:
									break;
								case scenes::MAIN_LOOP:
									MAINLOOPscene->Item_Draw();
									break;
								default:
									break;
								}
							}
						}, *cam_t);
					}
					//ディスプレイ描画
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
					{
						//FPS視点
						if (DrawPts->use_vr) {
							DrawPts->outScreen[0].DrawRotaGraph(960, 540, 0.5f, 0, false);
						}
						else {
							DrawPts->outScreen[0].DrawGraph(0, 0, false);
						}
						//TPS視点
						switch (sel_scene) {
						case scenes::ITEM_LOAD:
							break;
						case scenes::MAP_LOAD:
							break;
						case scenes::LOAD:
							break;
						case scenes::SELECT:
							break;
						case scenes::MAIN_LOOP:
							MAINLOOPscene->LAST_Draw();
							break;
						default:
							break;
						}
						//キー
						k_.draw();
						//デバッグ
						DebugPTs->end_way();
						DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}
				/*
				while (true) {
					if (float(GetNowHiPerformanceCount() - waits) / 1000.f > 33.34f) {
						break;
					}
				}
				//*/
				//画面の反映
				DrawPts->Screen_Flip();
				//終了判定
				if (!selend) {
					if (k_.key_use_ID[11].get_key(0)) {
						this->ending = false;
					}
					break;
				}
				//
			}
			//終了処理
			{
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					UI_LOADPTs->Dispose();
					MAINLOOPscene->first_after();
					break;
				case scenes::MAP_LOAD:
					UI_LOADPTs->Dispose();
					MAPPTs->Set_map(MAINLOOPscene, VGet(0.5f, -0.5f, 0.5f)/*MAINLOOPscene->get_Light_vec()*/);
					break;
				case scenes::LOAD:
					LOADscene->Dispose(&SELECTscene->preset);
					break;
				case scenes::SELECT:
					SELECTscene->Dispose();
					break;
				case scenes::MAIN_LOOP:
					MAINLOOPscene->Dispose();//解放
					MAPPTs->Delete_map();
					break;
				default:
					break;
				}
				DrawPts->Delete_Shadow();
			}
			//遷移
			switch (sel_scene) {
			case scenes::ITEM_LOAD:
				sel_scene = scenes::MAP_LOAD;
				break;
			case scenes::MAP_LOAD:
				sel_scene = scenes::LOAD;
				break;
			case scenes::LOAD:
				sel_scene = scenes::SELECT;
				break;
			case scenes::SELECT:
				sel_scene = scenes::MAIN_LOOP;
				break;
			case scenes::MAIN_LOOP:
				sel_scene = scenes::MAP_LOAD;//ITEM_LOADへは飛ばない
				break;
			default:
				break;
			}
			//
			if (!this->ending) {
				MAINLOOPscene->Dispose();//解放
				MAPPTs->Delete_map();
				break;
			}
			//
		} while (true);
		//
	}
};