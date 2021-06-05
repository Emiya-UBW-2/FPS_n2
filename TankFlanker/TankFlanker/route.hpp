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
	//�I�������t���O
	bool ending = true;
	int sel_scene = scenes::ITEM_LOAD;
	cam_info* cam_t = nullptr;
	bool update_effect_f = true;
	LONGLONG update_effect_was = 0;

	bool selend = true;
	bool selpause = true;
public:
	main_c(void) noexcept {
		auto OPTPTs = std::make_unique<OPTION>();																		//�ݒ�ǂݍ���
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);	//�ėp
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);															//�f�o�b�O
		if (DrawPts->use_vr) {
			//VR�ł͎g���Ȃ��@�\
			OPTPTs->DoF = false;
			OPTPTs->Bloom = false;
			OPTPTs->SSAO = false;
		}
		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		auto MAPPTs = std::make_unique<MAPclass::Map>(OPTPTs->grass_level);																	//MAP
		auto UI_LOADPTs = std::make_unique<UIclass::UI_LOADING>();																			//UI_LOADING
		UI_LOADPTs->Get_ptr(&DrawPts, &MAPPTs);
		//�L�[�ǂݍ���
		auto KeyBind = std::make_unique<key_bind>();
		auto PauseMenu = std::make_unique<pause_menu>(&KeyBind);
		//�V�[��
		auto MAINLOOPscene = std::make_unique<Sceneclass::MAINLOOP>(&MAPPTs, &OPTPTs);
		auto LOADscene = std::make_unique<Sceneclass::LOAD>();
		auto SELECTscene = std::make_unique<Sceneclass::SELECT>();

		LOADscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		SELECTscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		MAINLOOPscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		//�J��Ԃ�
		do {
			//�J�n
			{
				//
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					//�A�C�e���ǂݍ���
					UI_LOADPTs->Set("�A�C�e���f�[�^");
					break;
				case scenes::MAP_LOAD:
					MAPPTs->Ready_map("data/map_new2");
					//�}�b�v�ǂݍ���
					UI_LOADPTs->Set("�}�b�v");
					break;
				case scenes::LOAD:
					//�L�����ݒ�
					MAINLOOPscene->Set_Charaa(MAPPTs->get_spawn_point().size());
					//
					LOADscene->Set();
					DrawPts->Set_Light_Shadow(LOADscene->get_Shadow_maxpos(), LOADscene->get_Shadow_minpos(), LOADscene->get_Light_vec(), [&] {LOADscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(LOADscene->get_Light_color());
					//
					break;
				case scenes::SELECT:
					//
					SELECTscene->Set();
					DrawPts->Set_Light_Shadow(SELECTscene->get_Shadow_maxpos(), SELECTscene->get_Shadow_minpos(), SELECTscene->get_Light_vec(), [&] {SELECTscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(SELECTscene->get_Light_color());
					//
					break;
				case scenes::MAIN_LOOP:
					//
					MAINLOOPscene->Set();
					MAPPTs->Start_Ray(MAINLOOPscene->get_Light_vec());
					DrawPts->Set_Light_Shadow(MAINLOOPscene->get_Shadow_maxpos(), MAINLOOPscene->get_Shadow_minpos(), MAINLOOPscene->get_Light_vec(), [&] {MAINLOOPscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(MAINLOOPscene->get_Light_color());
					//
					break;
				default:
					break;
				}
				//
				selend = true;
				selpause = false;
				update_effect_was = GetNowHiPerformanceCount();
			}
			//��(���ʂ̂���)
			auto set_key_vr = [&]() {
#ifdef _USE_OPENVR_
				auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//�ˌ�
						mine_k.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																						//�}�O�L���b�`
						mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f&&ptr_->touch.y() < 0.5f&&ptr_->touch.y() > -0.5f);	//�Z���N�^�[
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);		//�}�K�W������
						mine_k.have_item = (ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0;		//�A�C�e���擾
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
					//�X�V
					KeyBind->reSet_isalways();
					cam_t = nullptr;//2D�@�\�����g���Ƃ���null
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::MAP_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::LOAD:
						selpause = false;
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
								//�ݒ�
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
							}
							//
							selend = LOADscene->UpDate();
						}
						//cam_t = &LOADscene->Get_Camera();
						break;
					case scenes::SELECT:
						selpause = false;
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
								//�ݒ�
								mine_k.wkey = KeyBind->get_key_use(0);
								mine_k.skey = KeyBind->get_key_use(1);
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
								mine_k.shoot = KeyBind->get_mouse_use(0);
								mine_k.select = KeyBind->get_mouse_use(1);
							}
							//
							selend = SELECTscene->UpDate();
						}
						cam_t = &SELECTscene->Get_Camera();
						break;
					case scenes::MAIN_LOOP:
						selpause = PauseMenu->Pause_key();
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
								//�ݒ�
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
								KeyBind->key_use_ID[9].on_off.first = mine_k.squat;	//�L��
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
							//
							selend = MAINLOOPscene->UpDate();//2~4ms
						}
						else {
							//�L�[�A�N�e�B�u
							PauseMenu->Pause_key_active();
							//
							selend = PauseMenu->Update();
						}
						cam_t = &MAINLOOPscene->Get_Camera();
						break;
					default:
						break;
					}
					//VR��ԂɓK�p
					DrawPts->Move_Player();
					//�`��
					{
						//UI��������
						HostpassPTs->Set_UI_draw([&](void) noexcept {
							switch (sel_scene) {
							case scenes::ITEM_LOAD:
								UI_LOADPTs->UI_Draw();
								break;
							case scenes::MAP_LOAD:
								UI_LOADPTs->UI_Draw();
								break;
							case scenes::LOAD:
								LOADscene->UI_Draw();
								break;
							case scenes::SELECT:
								SELECTscene->UI_Draw();
								break;
							case scenes::MAIN_LOOP:
								MAINLOOPscene->UI_Draw();//99->86
								break;
							default:
								break;
							}
						});
						if (cam_t != nullptr) {
							//���ʒu�w��
							Set3DSoundListenerPosAndFrontPosAndUpVec(cam_t->campos.get(), cam_t->camvec.get(), cam_t->camup.get());
							//�e�p��
							switch (sel_scene) {
							case scenes::ITEM_LOAD:
								break;
							case scenes::MAP_LOAD:
								break;
							case scenes::LOAD:
								DrawPts->Ready_Shadow(cam_t->campos, [&] { LOADscene->Shadow_Draw(); }, [&] { LOADscene->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));
								break;
							case scenes::SELECT:
								DrawPts->Ready_Shadow(cam_t->campos, [&] { SELECTscene->Shadow_Draw(); }, [&] { SELECTscene->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));
								break;
							case scenes::MAIN_LOOP:
								DrawPts->Ready_Shadow(cam_t->campos, [&] {MAINLOOPscene->Shadow_Draw(); }, [&] {MAINLOOPscene->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));
								//��near�͂���
								//	(MAINLOOPscene->Get_Mine().get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
								break;
							default:
								break;
							}
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							{
								update_effect_f = false;
								if (!selpause && (GetNowHiPerformanceCount() - update_effect_was) >= 1000000 / 60) {
									update_effect_f = true;
									update_effect_was = GetNowHiPerformanceCount();
								}
							}
							//VR�Ɉڂ�
							DrawPts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = *cam_t;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									switch (sel_scene) {
									case scenes::ITEM_LOAD:
										break;
									case scenes::MAP_LOAD:
										break;
									case scenes::LOAD:
										HostpassPTs->BUF_draw([&](void) noexcept { LOADscene->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { LOADscene->Main_Draw(); }); }, tmp_cam, update_effect_f);
										break;
									case scenes::SELECT:
										HostpassPTs->BUF_draw([&](void) noexcept { SELECTscene->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { SELECTscene->Main_Draw(); }); }, tmp_cam, update_effect_f);
										break;
									case scenes::MAIN_LOOP:
										HostpassPTs->BUF_draw([&](void) noexcept { MAINLOOPscene->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { MAINLOOPscene->Main_Draw(); }); }, tmp_cam, update_effect_f);
										break;
									default:
										break;
									}
									//�ŏI�`��
									HostpassPTs->Set_MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									HostpassPTs->Draw(cam_t, DrawPts->use_vr);
									//UI2
									switch (sel_scene) {
									case scenes::ITEM_LOAD:
										//UI_LOADPTs->Item_Draw();
										break;
									case scenes::MAP_LOAD:
										//UI_LOADPTs->Item_Draw();
										break;
									case scenes::LOAD:
										LOADscene->Item_Draw();
										break;
									case scenes::SELECT:
										SELECTscene->Item_Draw();
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
						else {
							//VR�Ɉڂ�
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
					//�f�B�X�v���C�`��
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
					{
						//�`��
						if (DrawPts->use_vr) {
							DrawPts->outScreen[0].DrawRotaGraph(960, 540, 0.5f, 0, false);
						}
						else {
							DrawPts->outScreen[0].DrawGraph(0, 0, false);
						}
						//��ɏ���
						switch (sel_scene) {
						case scenes::ITEM_LOAD:
							break;
						case scenes::MAP_LOAD:
							break;
						case scenes::LOAD:
							LOADscene->LAST_Draw();
							break;
						case scenes::SELECT:
							SELECTscene->LAST_Draw();
							break;
						case scenes::MAIN_LOOP:
							MAINLOOPscene->LAST_Draw();
							break;
						default:
							break;
						}
						//�|�[�Y
						PauseMenu->draw();
						//�L�[
						KeyBind->draw();
						//�f�o�b�O
						DebugPTs->end_way();
						DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}

				printfDx("call :%d", GetDrawCallCount());
				//��ʂ̔��f
				DrawPts->Screen_Flip();
				//�I������
				if (KeyBind->Esc_key()) {
					this->ending = false;
					break;
				}
				if (!selend) {
					break;
				}
				//
			}
			//�I������
			{
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					UI_LOADPTs->Dispose();
					MAINLOOPscene->Start_After();
					break;
				case scenes::MAP_LOAD:
					UI_LOADPTs->Dispose();
					MAPPTs->Start();
					break;
				case scenes::LOAD:
					LOADscene->Dispose();
					SELECTscene->preset = LOADscene->putout_preset();
					break;
				case scenes::SELECT:
					SELECTscene->Dispose();
					break;
				case scenes::MAIN_LOOP:
					MAINLOOPscene->Dispose();//���
					MAPPTs->Dispose();
					break;
				default:
					break;
				}
				DrawPts->Delete_Shadow();
			}
			//�J��
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
				sel_scene = scenes::MAP_LOAD;
				break;
			default:
				break;
			}
			//
			if (!this->ending) {
				MAINLOOPscene->Dispose();//���
				MAPPTs->Dispose();
				break;
			}
			//
		} while (true);
		//
	}
};