#pragma once
class main_c {
	//終了処理フラグ
	bool ending = true;
	int sel_scene = 0;
	GraphHandle UI_Screen;
	cam_info* cam_t=nullptr;
public:
	main_c(void) {
		auto OPTPTs = std::make_unique<OPTION>();																			//設定読み込み
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);		//汎用
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);																//デバッグ
		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, DrawPts->disp_x, DrawPts->disp_y);	//ホストパスエフェクト(VR、フルスクリーン共用)
		auto MAPPTs = std::make_unique<MAPclass::Map>();																	//MAP
		auto UI_LOADPTs = std::make_unique<UIclass::UI_Load>();																//UI_Load
		UI_Screen = GraphHandle::Make(DrawPts->disp_x, DrawPts->disp_y, true);												//VR、フルスクリーン共用
		//シーン
		auto SELECTscene = std::make_unique<Sceneclass::SELECT>();
		auto MAINLOOPscene = std::make_unique<Sceneclass::MAINLOOP>(DrawPts, OPTPTs, UI_LOADPTs);
		//繰り返し
		do {
			bool selend = true;
			//開始
			switch (sel_scene) {
			case 0:
				//マップ読み込み
				MAPPTs->Ready_map("data/map_new2");
				UI_LOADPTs->load_window("マップ");
				MAPPTs->Set_map("data/maps/Set.txt", MAINLOOPscene->get_item(), MAINLOOPscene->get_gun_data(), MAINLOOPscene->get_mag_data(), MAINLOOPscene->get_meds_data(), "data/grassput.bmp");
				//
				MAINLOOPscene->first(MAPPTs);
				SELECTscene->Set(MAPPTs, DrawPts, OPTPTs, MAINLOOPscene);
				break;
			case 1:
				MAINLOOPscene->Set(MAPPTs, DrawPts, OPTPTs);
				break;
			default:
				break;
			}
			//メイン
			while (ProcessMessage() == 0) {
				const auto waits = GetNowHiPerformanceCount();
				{
					DebugPTs->put_way();
					switch (sel_scene) {
					case 0:
						selend = SELECTscene->UpDate(MAPPTs, DrawPts, MAINLOOPscene);
						cam_t = &SELECTscene->get_camera();
						break;
					case 1:
						selend = MAINLOOPscene->UpDate(MAPPTs, DrawPts);
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
							case 0:
								SELECTscene->Shadow_Draw();
								break;
							case 1:
								MAINLOOPscene->Shadow_Draw();
								break;
							default:
								break;
							}

						}, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
						//↑nearはこれ
						//	(MAINLOOPscene->get_mine().HP != 0) ? VGet(2.f, 2.5f, 2.f) : VGet(10.f, 2.5f, 10.f)
						//書き込み
						this->UI_Screen.SetDraw_Screen();
						{
							switch (sel_scene) {
							case 0:
								SELECTscene->UI_Draw(MAINLOOPscene);
								break;
							case 1:
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
								HostpassPTs->BUF_draw(
									[&](void) {
									switch (sel_scene) {
									case 0:
										SELECTscene->BG_Draw();
										break;
									case 1:
										MAINLOOPscene->BG_Draw(MAPPTs);
										break;
									default:
										break;
									}

								}, [&](void) { DrawPts->Draw_by_Shadow([&] {
									switch (sel_scene) {
									case 0:
										SELECTscene->Main_Draw();
										break;
									case 1:
										MAINLOOPscene->Main_Draw(MAPPTs);
										break;
									default:
										break;
									}
								}); }, tmp_cam);
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
								case 0:

									break;
								case 1:
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
						case 0:
							break;
						case 1:
							MAINLOOPscene->LAST_Draw();
							break;
						default:
							break;
						}
						//デバッグ
						DebugPTs->end_way();
						DebugPTs->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}
				//画面の反映
				DrawPts->Screen_Flip();
				//終了判定
				if (!selend) {
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
					}
					break;
				}
				//
			}
			//終了処理
			{
				switch (sel_scene) {
				case 0:
					SELECTscene->Dispose();
					break;
				case 1:
					MAINLOOPscene->Dispose(MAPPTs);//解放
					break;
				default:
					break;
				}
				DrawPts->Delete_Shadow();
			}
			//遷移
			switch (sel_scene) {
			case 0:
				sel_scene = 1;
				break;
			case 1:
				sel_scene = 0;
				break;
			default:
				break;
			}
			//
			if (!this->ending) {
				MAINLOOPscene->Dispose(MAPPTs);//解放
				break;
			}
			//
		} while (true);
		//
	}
};