#pragma once
class main_c {
	//終了処理フラグ
	bool ending = true;
	int sel_scene = 0;
	GraphHandle UI_Screen;
	cam_info* cam_t=nullptr;
public:
	main_c() {
		auto OPTPTs = std::make_unique<OPTION>();																			//設定読み込み
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->shadow, OPTPTs->vsync);		//汎用
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);																//デバッグ
		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->bloom, DrawPts->disp_x, DrawPts->disp_y);	//ホストパスエフェクト(VR、フルスクリーン共用)
		auto MAPPTs = std::make_unique<MAPclass::Map>();																	//MAP
		UI_Screen = GraphHandle::Make(DrawPts->disp_x, DrawPts->disp_y, true);												//VR、フルスクリーン共用
		//シーン
		auto SELECTparts = std::make_unique<Sceneclass::SELECT>();
		auto MAINLOOPparts = std::make_unique<Sceneclass::MAINLOOP>(OPTPTs);
		//繰り返し
		do {
			bool selend = true;
			//開始
			switch (sel_scene) {
			case 0:
				MAINLOOPparts->first(MAPPTs);
				SELECTparts->Set(MAPPTs, DrawPts, OPTPTs, MAINLOOPparts);
				break;
			case 1:
				MAINLOOPparts->Set(MAPPTs, DrawPts, OPTPTs);
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
						selend = SELECTparts->update(MAPPTs, DrawPts, MAINLOOPparts);
						cam_t = &SELECTparts->get_camera();
						break;
					case 1:
						selend = MAINLOOPparts->update(MAPPTs, DrawPts, OPTPTs);
						cam_t = &MAINLOOPparts->get_camera();
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
								SELECTparts->Shadow_Draw();
								break;
							case 1:
								MAINLOOPparts->Shadow_Draw();
								break;
							default:
								break;
							}

						}, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
						//↑nearはこれ
						//	(MAINLOOPparts->get_mine().HP != 0) ? VGet(2.f, 2.5f, 2.f) : VGet(10.f, 2.5f, 10.f)
						//書き込み
						this->UI_Screen.SetDraw_Screen();
						{
							switch (sel_scene) {
							case 0:
								SELECTparts->UI_Draw(MAINLOOPparts);
								break;
							case 1:
								MAINLOOPparts->UI_Draw(DrawPts);
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
									[&]() {
									switch (sel_scene) {
									case 0:
										SELECTparts->BG_Draw();
										break;
									case 1:
										MAINLOOPparts->BG_Draw(MAPPTs);
										break;
									default:
										break;
									}

								}, [&]() { DrawPts->Draw_by_Shadow([&] {
									switch (sel_scene) {
									case 0:
										SELECTparts->Main_Draw();
										break;
									case 1:
										MAINLOOPparts->Main_Draw(MAPPTs);
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
									MAINLOOPparts->Item_Draw(DrawPts);
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
							MAINLOOPparts->LAST_Draw();
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
			switch (sel_scene) {
			case 0:
				SELECTparts->Dispose(DrawPts);
				break;
			case 1:
				MAINLOOPparts->Dispose(MAPPTs, DrawPts);//解放
				break;
			default:
				break;
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
				for (auto& c : MAINLOOPparts->get_chara()) {
					c.Delete_chara();
				}
				MAINLOOPparts->get_chara().clear();
				for (auto& i : MAINLOOPparts->get_item()) {
					i.Delete_item();
				}
				MAINLOOPparts->get_item().clear();
				MAPPTs->Delete_map();
				break;
			}
			//
		} while (true);
		//
	}
};