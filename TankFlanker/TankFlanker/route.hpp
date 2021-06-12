#pragma once
enum scenes
{
	NONE_SCENE,
	ITEM_LOAD,
	MAP_LOAD,
	LOAD,
	SELECT,
	MAIN_LOOP
};

class main_c {
	//終了処理フラグ
	bool ending = true;
	int sel_scene = scenes::NONE_SCENE;
	cam_info* cam_t = nullptr;
	bool update_effect_f = true;
	LONGLONG update_effect_was = 0;

	bool selend = true;
	bool selpause = true;

public:
	main_c(void) noexcept {
		auto OPTPTs = std::make_unique<OPTION>();																		//設定読み込み
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);	//汎用
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);															//デバッグ
		if (DrawPts->use_vr) {
			//VRでは使えない機能
			OPTPTs->DoF = false;
			OPTPTs->Bloom = false;
			OPTPTs->SSAO = false;
		}
		//int PixelShaderHandle;
		//int VertexShaderHandle;
		//VertexShaderHandle = LoadVertexShader("shader/NormalMesh_PointLightVS.vso");			// 頂点シェーダーを読み込む
		//PixelShaderHandle = LoadPixelShader("shader/NormalMesh_PointLightPS.pso");		// ピクセルシェーダーを読み込む
		//SetUseVertexShader(VertexShaderHandle);	// 使用する頂点シェーダーをセット
		//SetUsePixelShader(PixelShaderHandle);	// 使用するピクセルシェーダーをセット
		//MV1SetUseOrigShader(TRUE);	// モデルの描画にオリジナルシェーダーを使用する設定をＯＮにする

		VERTEX3DSHADER Screen_vertex[6] = { 0.0f };

		// 頂点データの準備
		int xp1 = 0;
		int yp1 = 0;
		int xp2 = deskx;
		int yp2 = desky;
		Screen_vertex[0].pos = VGet(float(xp1), float(desky - yp1), 0.0f);
		Screen_vertex[1].pos = VGet(float(xp2), float(desky - yp1), 0.0f);
		Screen_vertex[2].pos = VGet(float(xp1), float(desky - yp2), 0.0f);
		Screen_vertex[3].pos = VGet(float(xp2), float(desky - yp2), 0.0f);
		Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
		Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
		Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
		Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
		Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
		Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
		Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
		Screen_vertex[4] = Screen_vertex[2];
		Screen_vertex[5] = Screen_vertex[1];


		FLOAT2 *dispsize;
		FLOAT4 *f4;

		//レンズ
		int pshandle, vshandle;
		int pscbhandle;
		int pscbhandle2;
		vshandle = LoadVertexShader("shader/VS_lens.vso");	// 頂点シェーダーバイナリコードの読み込み
		pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
		pshandle = LoadPixelShader("shader/PS_lens.pso");		// ピクセルシェーダーバイナリコードの読み込み

		//歪み
		int pshandle_l, vshandle_l;
		int pscbhandle_l;
		int pscbhandle2_l;
		vshandle_l = LoadVertexShader("shader/ShaderPolygon3DTestVS.vso");	// 頂点シェーダーバイナリコードの読み込み
		pscbhandle_l = CreateShaderConstantBuffer(sizeof(float) * 4);
		pscbhandle2_l = CreateShaderConstantBuffer(sizeof(float) * 4);
		pshandle_l = LoadPixelShader("shader/ShaderPolygon3DTestPS.pso");		// ピクセルシェーダーバイナリコードの読み込み

		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//ホストパスエフェクト(VR、フルスクリーン共用)
		auto MAPPTs = std::make_unique<MAPclass::Map>(OPTPTs->grass_level);																	//MAP
		//キー読み込み
		auto KeyBind = std::make_unique<key_bind>();
		auto PauseMenu = std::make_unique<pause_menu>(&KeyBind);
		//シーン
		auto UI_LOADPTs = std::make_shared<Sceneclass::LOADING>();
		auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(&MAPPTs, &OPTPTs);
		auto LOADscene = std::make_shared<Sceneclass::LOAD>();
		auto SELECTscene = std::make_shared<Sceneclass::SELECT>();
		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
		//
		UI_LOADPTs->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		LOADscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		SELECTscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		MAINLOOPscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		//
		//繰り返し
		do {
			//遷移
			switch (sel_scene) {
				default:
					UI_LOADPTs->settitle("アイテムデータ");							//アイテム読み込み
					sel_scene = scenes::ITEM_LOAD;
					scenes_ptr = UI_LOADPTs;
					break;
				case scenes::ITEM_LOAD:
					MAINLOOPscene->Start_After();
					MAPPTs->Ready_map("data/map_new2");								//マップ読み込み
					UI_LOADPTs->settitle("マップ");									//マップ読み込み
					sel_scene = scenes::MAP_LOAD;
					scenes_ptr = UI_LOADPTs;
					break;
				case scenes::MAP_LOAD:
					MAPPTs->Start();
					MAINLOOPscene->Set_Charaa(MAPPTs->get_spawn_point().size());	//キャラ設定
					sel_scene = scenes::LOAD;
					scenes_ptr = LOADscene;
					break;
				case scenes::LOAD:
					SELECTscene->preset = LOADscene->putout_preset();
					sel_scene = scenes::SELECT;
					scenes_ptr = SELECTscene;
					break;
				case scenes::SELECT:
					sel_scene = scenes::MAIN_LOOP;
					scenes_ptr = MAINLOOPscene;
					break;
				case scenes::MAIN_LOOP:
					MAPPTs->Dispose();
					MAPPTs->Ready_map("data/map_new2");								//マップ読み込み
					UI_LOADPTs->settitle("マップ");							//マップ読み込み
					sel_scene = scenes::MAP_LOAD;
					scenes_ptr = UI_LOADPTs;
					break;
				}
			//開始
			{
				scenes_ptr->Set();
				DrawPts->Set_Light_Shadow(scenes_ptr->get_Shadow_maxpos(), scenes_ptr->get_Shadow_minpos(), scenes_ptr->get_Light_vec(), [&] {scenes_ptr->Shadow_Draw_Far(); });
				SetGlobalAmbientLight(scenes_ptr->get_Light_color());
				if (sel_scene == scenes::MAIN_LOOP) { MAPPTs->Start_Ray(scenes_ptr->get_Light_vec()); }
				selend = true;
				selpause = false;
				update_effect_was = GetNowHiPerformanceCount();
			}
			//仮(共通のため)
			auto set_key_vr = [&]() {
#ifdef _USE_OPENVR_
				auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//射撃
						mine_k.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																						//マグキャッチ
						mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f&&ptr_->touch.y() < 0.5f&&ptr_->touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);		//マガジン持つ
						mine_k.have_item = (ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0;		//アイテム取得
						mine_k.sort_magazine = false;									//
						mine_k.view_gun = false;
						mine_k.drop_ = false;											//
						mine_k.throw_gre = false;										//
						mine_k.running = (ptr_->on[0] & BUTTON_TOUCHPAD) != 0;			//running
						mine_k.jamp = (ptr_->on[0] & BUTTON_SIDE) != 0;					//jamp
					}
				}
#endif // _USE_OPENVR_
			};
			//
			while (ProcessMessage() == 0) {
				clsDx();
				const auto waits = GetNowHiPerformanceCount();
				FPS = GetFPS();
				DebugPTs->put_way();
				{
					//更新
					KeyBind->reSet_isalways();
					selpause = false;
					switch (sel_scene) {
					case scenes::LOAD:
						if (!selpause) {
							//キーアクティブ
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
								//設定
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
							}
						}
						break;
					case scenes::SELECT:
						if (!selpause) {
							//キーアクティブ
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
								//設定
								mine_k.wkey = KeyBind->get_key_use(0);
								mine_k.skey = KeyBind->get_key_use(1);
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
								mine_k.shoot = KeyBind->get_mouse_use(0);
								mine_k.select = KeyBind->get_mouse_use(1);
							}
						}
						break;
					case scenes::MAIN_LOOP:
						selpause = PauseMenu->Pause_key();
						if (!selpause) {
							//キーアクティブ
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
								//設定
								mine_k.have_mag = true;
								mine_k.wkey = KeyBind->get_key_use(0);
								mine_k.skey = KeyBind->get_key_use(1);
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.qkey = KeyBind->get_key_use(4);
								mine_k.ekey = KeyBind->get_key_use(5);
								mine_k.reload = KeyBind->get_key_use(6);
								mine_k.have_item = KeyBind->get_key_use(7);
								mine_k.throw_gre = KeyBind->get_key_use(8);
								KeyBind->key_use_ID[9].on_off.first = mine_k.squat;	//記憶
								mine_k.squat = KeyBind->get_key_use(9);
								mine_k.sort_magazine = KeyBind->get_key_use(12);
								mine_k.running = KeyBind->get_key_use(13);
								mine_k.jamp = KeyBind->get_key_use(14);
								mine_k.TPS = KeyBind->get_key_use(15);
								mine_k.view_gun = KeyBind->get_key_use(17);
								mine_k.drop_ = KeyBind->get_key_use(19);
								mine_k.shoot = KeyBind->get_mouse_use(0);
								mine_k.select = KeyBind->get_mouse_use(1);
								mine_k.aim = KeyBind->get_mouse_use(2);
							}
						}
						else {
							//キーアクティブ
							PauseMenu->Pause_key_active();
							//
							selend = PauseMenu->Update();
						}
						break;
					default:
						break;
					}
					if (!selpause) {
						selend = scenes_ptr->UpDate();
					}
					cam_t = &scenes_ptr->Get_Camera();
					//VR空間に適用
					DrawPts->Move_Player();
					//描画
					{
						//UI書き込み
						HostpassPTs->Set_UI_draw([&](void) noexcept { scenes_ptr->UI_Draw(); });
						if (cam_t != nullptr) {
							//音位置指定
							Set3DSoundListenerPosAndFrontPosAndUpVec(cam_t->campos.get(), cam_t->camvec.get(), cam_t->camup.get());
							//影用意
							DrawPts->Ready_Shadow(cam_t->campos, [&] { scenes_ptr->Shadow_Draw(); }, [&] { scenes_ptr->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));
							//scenes::MAIN_LOOP:
							//↑nearはこれ
							//	(scenes_ptr->Get_Mine()->get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
							//エフェクシアのアプデを60FPS相当に変更
							{
								update_effect_f = false;
								if (!selpause && (GetNowHiPerformanceCount() - update_effect_was) >= 1000000 / 60) {
									update_effect_f = true;
									update_effect_was = GetNowHiPerformanceCount();
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
									HostpassPTs->BUF_draw([&](void) noexcept { scenes_ptr->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, update_effect_f);
									//最終描画
									HostpassPTs->Set_MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp);
								{
									float zoom = scenes_ptr->zoom_lens();
									float reticle_size = scenes_ptr->size_lens();
									if (scenes_ptr->is_lens())
									{
										//レンズ描画
										SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());		// 使用するテクスチャをセット
										dispsize = (FLOAT2 *)GetBufferShaderConstantBuffer(pscbhandle);			// ピクセルシェーダー用の定数バッファのアドレスを取得
										dispsize->u = float(DrawPts->disp_x);
										dispsize->v = float(DrawPts->disp_y);
										UpdateShaderConstantBuffer(pscbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
										SetShaderConstantBuffer(pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
										{
											f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(pscbhandle2);			// ピクセルシェーダー用の定数バッファのアドレスを取得
											f4->x = float(DrawPts->disp_x) / 2.f;
											f4->y = float(DrawPts->disp_y) / 2.f;
											f4->z = reticle_size;
											f4->w = zoom;
										}
										UpdateShaderConstantBuffer(pscbhandle2);							// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
										SetShaderConstantBuffer(pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット

										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											SetUseVertexShader(vshandle);		// 使用する頂点シェーダーをセット
											SetUsePixelShader(pshandle);		// 使用するピクセルシェーダーをセット
											MV1SetUseOrigShader(TRUE);
											DrawPolygon3DToShader(Screen_vertex, 2);		// 描画
											MV1SetUseOrigShader(FALSE);
										}
										HostpassPTs->Get_MAIN_Screen().SetDraw_Screen();
										{
											HostpassPTs->Get_BUF_Screen().DrawGraph(0, 0, true);
										}
									}

									float bless_ratio = scenes_ptr->ratio_bless();
									float bless = scenes_ptr->time_bless();
									if (scenes_ptr->is_bless())
									{
										//歪み描画
										SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());		// 使用するテクスチャをセット
										dispsize = (FLOAT2 *)GetBufferShaderConstantBuffer(pscbhandle_l);			// ピクセルシェーダー用の定数バッファのアドレスを取得
										dispsize->u = float(DrawPts->disp_x);
										dispsize->v = float(DrawPts->disp_y);
										UpdateShaderConstantBuffer(pscbhandle_l);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
										SetShaderConstantBuffer(pscbhandle_l, DX_SHADERTYPE_PIXEL, 2);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
										{
											f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(pscbhandle2_l);			// ピクセルシェーダー用の定数バッファのアドレスを取得
											f4->x = 0.f;
											f4->y = 0.f;
											f4->z = bless_ratio;
											f4->w = bless_ratio * (1.f - cos(bless)) / 2.f;
										}
										UpdateShaderConstantBuffer(pscbhandle2_l);							// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
										SetShaderConstantBuffer(pscbhandle2_l, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット

										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											SetUseVertexShader(vshandle_l);		// 使用する頂点シェーダーをセット
											SetUsePixelShader(pshandle_l);		// 使用するピクセルシェーダーをセット
											MV1SetUseOrigShader(TRUE);
											DrawPolygon3DToShader(Screen_vertex, 2);		// 描画
											MV1SetUseOrigShader(FALSE);
										}
										HostpassPTs->Get_MAIN_Screen().SetDraw_Screen();
										{
											HostpassPTs->Get_BUF_Screen().DrawGraph(0, 0, true);
										}
									}

								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									//デフォ描画
									HostpassPTs->MAIN_draw();
									//
									HostpassPTs->Draw(cam_t, DrawPts->use_vr);
									//UI2
									scenes_ptr->Item_Draw();
								}
							}, *cam_t);
						}
						else {
							//VRに移す
							cam_info tmp_cam;
							DrawPts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								GraphHandle::SetDraw_Screen(tmp);
								{
									HostpassPTs->UI_draw();
								}
							}, tmp_cam);
						}
					}
					//ディスプレイ描画
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
					{
						//描画
						if (DrawPts->use_vr) {
							DrawPts->outScreen[0].DrawRotaGraph(960, 540, 0.5f, 0, false);
						}
						else {
							DrawPts->outScreen[0].DrawGraph(0, 0, false);
						}
						//上に書く
						scenes_ptr->LAST_Draw();
						//ポーズ
						PauseMenu->draw();
						//キー
						KeyBind->draw();
						//デバッグ
						DebugPTs->end_way();
						DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}

				printfDx("call :%d", GetDrawCallCount());
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
				DrawPts->Delete_Shadow();
			}
			//
			if (!this->ending) {
				MAINLOOPscene->Dispose();//解放
				MAPPTs->Dispose();
				break;
			}
			//
		} while (true);
		//
	}
};