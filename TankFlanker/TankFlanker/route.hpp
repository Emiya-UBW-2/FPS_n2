#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class main_c {
	private:
		bool ending{ true };							//終了処理フラグ
		scenes sel_scene{ scenes::NONE_SCENE };			//現在のシーン
		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
		bool selend{ true };
		bool selpause{ true };
		//シェーダー
		shaders::shader_Vertex Screen_vertex;					// 頂点データ
		std::array<shaders, 2> shader2D;
	public:
		main_c(void) noexcept {
			auto OPTPTs = std::make_shared<OPTION>();								//設定読み込み
			auto DrawPts = std::make_shared<DXDraw>("FPS_n2", OPTPTs, Frame_Rate);		//汎用
#ifdef DEBUG
			auto DebugPTs = std::make_shared<DeBuG>(Frame_Rate);					//デバッグ
#endif // DEBUG
			OPTPTs->Set_useVR(DrawPts->use_vr);
			//エフェクト
			effectControl.Init();
			//シェーダー
			auto HostpassPTs = std::make_shared<HostPassEffect>(OPTPTs, DrawPts->disp_x, DrawPts->disp_y);				//ホストパスエフェクト(VR、フルスクリーン共用)
			//シェーダー
			Screen_vertex.Set(DrawPts);			// 頂点データの準備
			shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//レンズ
			shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");									//歪み
			//MAP
			auto MAPPTs = std::make_shared<MAPclass::Map>(OPTPTs->Get_grass_level(), DrawPts->disp_x, DrawPts->disp_y);
			MAPPTs->Set_mine(MAPPTs);
			//キー読み込み
			auto KeyBind = std::make_shared<key_bind>(DrawPts);
			auto PauseMenu = std::make_unique<pause_menu>(KeyBind, DrawPts);
			//シーン
			auto UI_LOADPTs = std::make_shared<Sceneclass::LOADING>();
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(MAPPTs, OPTPTs);
			auto LOADscene = std::make_shared<Sceneclass::LOAD>();
			auto SELECTscene = std::make_shared<Sceneclass::SELECT>();
			//リソース
			auto Audio_resource = std::make_shared<Audio_Control>();						//オーディオ
			auto GunPartses = std::make_shared<GUNPARTS_Control>();							//銃パーツ
			effectControl.Start();
			//
			UI_LOADPTs->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			LOADscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			SELECTscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			MAINLOOPscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			//遷移先指定
			LOADscene->Set_Next(SELECTscene, scenes::SELECT);
			SELECTscene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			MAINLOOPscene->Set_Next(UI_LOADPTs, scenes::MAP_LOAD);

			//繰り返し
			do {
				//遷移
				{
					//遷移前処理
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						scenes_ptr->Set_Next(UI_LOADPTs, scenes::MAP_LOAD);
						break;
					case scenes::MAP_LOAD:
						scenes_ptr->Set_Next(LOADscene, scenes::LOAD);
						break;
					default: break;
					}
					//遷移処理
					switch (sel_scene) {
					case scenes::NONE_SCENE://start
						sel_scene = scenes::ITEM_LOAD;
						scenes_ptr = UI_LOADPTs;
						break;
					default:
						sel_scene = scenes_ptr->Next_scene;
						scenes_ptr = scenes_ptr->Next_scenes_ptr;
						break;
					}
					//遷移後処理
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						UI_LOADPTs->settitle("アイテムデータ");	//アイテム読み込み
						break;
					case scenes::MAP_LOAD:
						UI_LOADPTs->settitle("マップ");				//マップ読み込み
						MAPPTs->Ready_map("data/map");				//マップ読み込み
						break;
					default: break;
					}
				}
				//開始
				{
					scenes_ptr->Set();
					if (sel_scene == scenes::MAIN_LOOP) {
						MAPPTs->Start_Ray(scenes_ptr->Get_Light_vec());
					}
					selend = true;
					selpause = false;
				}
				//
				while (ProcessMessage() == 0) {
#ifdef DEBUG
					clsDx();
#endif // DEBUG
					const auto waits = GetNowHiPerformanceCount();
					FPS = GetFPS();
#ifdef DEBUG
					DebugPTs->put_way();
#endif // DEBUG
					{
						//更新
						{
							KeyBind->Reset_isalways();
							selpause = false;
							if (sel_scene == scenes::MAIN_LOOP) {
								selpause = PauseMenu->Pause_key();
							}
							if (!selpause) {
								if (DrawPts->use_vr) {
									scenes_ptr->KeyOperation_VR();
								}
								else {
									scenes_ptr->KeyOperation();
								}
							}
							selend = (!selpause) ? scenes_ptr->Update() : PauseMenu->Update();
						}
						//VR空間に適用
						DrawPts->Move_Player();
						//描画
						{
							//エフェクシアのアプデを60FPS相当に変更
							if (!selpause) {
								effectControl.Calc();
							}
							//共通の描画前用意
							scenes_ptr->ReadyDraw();
							//UI書き込み
							HostpassPTs->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VRに移す
							DrawPts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									HostpassPTs->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//最終描画
									HostpassPTs->Set_MAIN_Draw();
								}
								//*
								GraphHandle::SetDraw_Screen(tmp);
								{
									SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());	//使用するテクスチャをセット
									if (scenes_ptr->is_lens()) {
										//レンズ描画
										shader2D[0].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[0].set_param(float(DrawPts->disp_x) / 2.f, float(DrawPts->disp_y) / 2.f, scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[0].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}

									if (scenes_ptr->is_bless()) {
										//歪み描画
										shader2D[1].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[1].set_param(0, 0, scenes_ptr->ratio_bless(), (1.f - cos(scenes_ptr->time_bless())) / 2.f);
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[1].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}
									SetUseTextureToShader(0, -1);	//使用するテクスチャをセット
								}
								//*/
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									HostpassPTs->MAIN_Draw();											//デフォ描画
									HostpassPTs->DrawUI(&scenes_ptr->Get_Camera(), DrawPts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//ディスプレイ描画
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
							//描画
							if (DrawPts->use_vr) {
								DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawPts->outScreen[0].DrawRotaGraph(DrawPts->disp_x / 2, DrawPts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawPts->outScreen[0].DrawGraph(0, 0, false);
							}
							//上に書く
							scenes_ptr->LAST_Draw();
							//ポーズ
							PauseMenu->Draw();
							//キー
							KeyBind->Draw();
							//デバッグ
#ifdef DEBUG
							DebugPTs->end_way();
							DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
#endif // DEBUG
						}
					}
					//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
#ifdef DEBUG
					printfDx("call  :%d\n", GetDrawCallCount());
					printfDx("Async :%d\n", GetASyncLoadNum());
#endif // DEBUG
					//画面の反映
					DrawPts->Screen_Flip();
					//終了判定
					if (KeyBind->Esc_key()) {
						this->ending = false;
						break;
					}
					if (!selend) {
						break;
					}
					//
				}
				//終了処理
				{
					//解放
					scenes_ptr->Dispose();
					//次ループの処理
					switch (sel_scene) {
					default: break;
					case scenes::ITEM_LOAD:
						GunPartses->Set();																//
						MAINLOOPscene->Start();															//メインループ開始読み込み
						break;
					case scenes::MAP_LOAD:
						MAPPTs->Start();																//マップパーツ生成
						MAINLOOPscene->Ready_Chara(MAPPTs->Get_spawn_point().size());					//キャラ設定
						LOADscene->Start(MAINLOOPscene->Get_Mine());									//
						break;
					case scenes::LOAD:
						SELECTscene->Start(LOADscene->putout_Preset(), MAINLOOPscene->Get_Mine());		//プリセットを指定
						break;
					case scenes::SELECT:
						MAINLOOPscene->Ready_Tank(2);													//戦車指定
						break;
					case scenes::MAIN_LOOP:
						MAPPTs->Dispose();																//マップを消去
						break;
					}
				}
				//
			} while (this->ending);
			//
			//MAINLOOPscene->Dispose();//解放
			MAPPTs->Dispose();
			DrawPts->Delete_Shadow();
			effectControl.Dispose();
		}
	};
};