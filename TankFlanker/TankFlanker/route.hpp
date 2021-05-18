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
public:
	main_c(void) {
		auto OPTPTs = std::make_unique<OPTION>();																		//�ݒ�ǂݍ���
		auto DrawPts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);	//�ėp
		auto DebugPTs = std::make_unique<DeBuG>(FRAME_RATE);															//�f�o�b�O
		if (DrawPts->use_vr) {
			OPTPTs->DoF = false;
			OPTPTs->Bloom = false;
			OPTPTs->SSAO = false;//SSAO��VR�ł͎g���Ȃ�
		}
		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		auto MAPPTs = std::make_unique<MAPclass::Map>();																					//MAP
		auto UI_LOADPTs = std::make_unique<UIclass::UI_LOADING>();																			//UI_LOADING
		//�L�[�ǂݍ���
		auto KeyBind = std::make_unique<key_bind>();
		//�V�[��
		auto MAINLOOPscene = std::make_unique<Sceneclass::MAINLOOP>(&DrawPts, &MAPPTs, &OPTPTs);
		auto LOADscene = std::make_unique<Sceneclass::LOAD>(DrawPts->use_vr,&MAINLOOPscene);
		auto SELECTscene = std::make_unique<Sceneclass::SELECT>(&DrawPts, &MAPPTs, &OPTPTs, &MAINLOOPscene);
		//�J��Ԃ�
		do {
			bool selend = true;
			//�J�n
			{
				//�펞�\���̃��Z�b�g
				KeyBind->reSet_isalways();
				//
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					//�A�C�e���ǂݍ���
					UI_LOADPTs->Set("�A�C�e���f�[�^");
					break;
				case scenes::MAP_LOAD:
					//�}�b�v�ǂݍ���
					MAPPTs->Ready_map("data/map_new2");
					UI_LOADPTs->Set("�}�b�v");
					break;
				case scenes::LOAD:
					//�L�����ݒ�
					UI_LOADPTs->Set("�}�b�v");
					MAINLOOPscene->Set_Charaa(MAPPTs->get_spawn_point().size());
					LOADscene->Set();
					if (DrawPts->use_vr) {
					}
					else {
						KeyBind->key_use_ID[2].isalways = true;
						KeyBind->key_use_ID[3].isalways = true;
						KeyBind->key_use_ID[14].isalways = true;
					}
					break;
				case scenes::SELECT:
					//
					SELECTscene->Set();
					DrawPts->Set_Light_Shadow(SELECTscene->get_Shadow_maxpos(), SELECTscene->get_Shadow_minpos(), SELECTscene->get_Light_vec(), [&] {SELECTscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(SELECTscene->get_Light_color());
					if (DrawPts->use_vr) {
					}
					else {
						KeyBind->key_use_ID[0].isalways = true;
						KeyBind->key_use_ID[1].isalways = true;
						KeyBind->key_use_ID[2].isalways = true;
						KeyBind->key_use_ID[3].isalways = true;
						KeyBind->key_use_ID[14].isalways = true;
						KeyBind->mouse_use_ID[0].isalways = true;
						KeyBind->mouse_use_ID[1].isalways = true;
					}
					break;
				case scenes::MAIN_LOOP:
					MAINLOOPscene->Set();
					DrawPts->Set_Light_Shadow(MAINLOOPscene->get_Shadow_maxpos(), MAINLOOPscene->get_Shadow_minpos(), MAINLOOPscene->get_Light_vec(), [&] {MAINLOOPscene->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(MAINLOOPscene->get_Light_color());
					if (DrawPts->use_vr) {
					}
					else {
						KeyBind->key_use_ID[0].isalways = true;
						KeyBind->key_use_ID[1].isalways = true;
						KeyBind->key_use_ID[2].isalways = true;
						KeyBind->key_use_ID[3].isalways = true;
						KeyBind->key_use_ID[4].isalways = true;
						KeyBind->key_use_ID[5].isalways = true;
						KeyBind->key_use_ID[6].isalways = true;
						KeyBind->key_use_ID[7].isalways = true;
						KeyBind->key_use_ID[8].isalways = true;
						KeyBind->key_use_ID[9].isalways = true;
						KeyBind->key_use_ID[10].isalways = true;
						KeyBind->key_use_ID[12].isalways = true;
						KeyBind->key_use_ID[13].isalways = true;
						KeyBind->key_use_ID[14].isalways = true;
						KeyBind->key_use_ID[15].isalways = true;
						KeyBind->key_use_ID[16].isalways = true;
						KeyBind->mouse_use_ID[0].isalways = true;
						KeyBind->mouse_use_ID[1].isalways = true;
						KeyBind->mouse_use_ID[2].isalways = true;
					}
					break;
				default:
					break;
				}
			}
			while (ProcessMessage() == 0) {
				//clsDx();
				const auto waits = GetNowHiPerformanceCount();
				DebugPTs->put_way();
				{
					//�X�V
					cam_t = nullptr;//2D�@�\�����g���Ƃ���null
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::MAP_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::LOAD:
						if (DrawPts->use_vr) {
							auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
							if (DrawPts->get_hand1_num() != -1) {
								auto ptr_ = DrawPts->get_device_hand1();
								if (ptr_->turn && ptr_->now) {
									mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																				//�ˌ�
									mine_k.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																					//�}�O�L���b�`
									mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f&&ptr_->touch.y() < 0.5f&&ptr_->touch.y() > -0.5f);	//�Z���N�^�[
								}
							}
							if (DrawPts->get_hand2_num() != -1) {
								auto ptr_ = DrawPts->get_device_hand2();
								if (ptr_->turn && ptr_->now) {
									mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);	//�}�K�W������
									mine_k.have_item = (ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0;		//�A�C�e���擾
									mine_k.sort_magazine = false;									//
									mine_k.drop_ = false;											//
									mine_k.running = (ptr_->on[0] & BUTTON_TOUCHPAD) != 0;			//running
									mine_k.jamp = (ptr_->on[0] & BUTTON_SIDE) != 0;					//jamp
								}
							}
						}
						else {
							auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
							//�L��
							KeyBind->key_use_ID[9].on_off.first = mine_k.squat;
							//�ݒ�
							mine_k.wkey = KeyBind->key_use_ID[0].get_key(0);
							mine_k.skey = KeyBind->key_use_ID[1].get_key(0);
							mine_k.dkey = KeyBind->key_use_ID[2].get_key(0);
							mine_k.akey = KeyBind->key_use_ID[3].get_key(0);
							mine_k.qkey = KeyBind->key_use_ID[4].get_key(0);
							mine_k.ekey = KeyBind->key_use_ID[5].get_key(0);
							mine_k.reload = KeyBind->key_use_ID[6].get_key(0);
							mine_k.have_item = KeyBind->key_use_ID[7].get_key(2);
							mine_k.drop_ = KeyBind->key_use_ID[8].get_key(2);
							mine_k.squat = KeyBind->key_use_ID[9].get_key(1);
							mine_k.sort_magazine = KeyBind->key_use_ID[12].get_key(2);
							mine_k.running = KeyBind->key_use_ID[13].get_key(0);
							mine_k.jamp = KeyBind->key_use_ID[14].get_key(2);
							mine_k.shoot = KeyBind->mouse_use_ID[0].get_key(3);
							mine_k.select = KeyBind->mouse_use_ID[1].get_key(5);
							mine_k.aim = KeyBind->mouse_use_ID[2].get_key(3);
							mine_k.have_mag = true;
						}
						selend = LOADscene->UpDate();
						break;
					case scenes::SELECT:
						//
						if (DrawPts->use_vr) {
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
									mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);	//�}�K�W������
									mine_k.have_item = (ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0;		//�A�C�e���擾
									mine_k.sort_magazine = false;									//
									mine_k.drop_ = false;											//
									mine_k.running = (ptr_->on[0] & BUTTON_TOUCHPAD) != 0;			//running
									mine_k.jamp = (ptr_->on[0] & BUTTON_SIDE) != 0;					//jamp
								}
							}
						}
						else {
							auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
							//�L��
							KeyBind->key_use_ID[9].on_off.first = mine_k.squat;
							//�ݒ�
							mine_k.wkey = KeyBind->key_use_ID[0].get_key(0);
							mine_k.skey = KeyBind->key_use_ID[1].get_key(0);
							mine_k.dkey = KeyBind->key_use_ID[2].get_key(0);
							mine_k.akey = KeyBind->key_use_ID[3].get_key(0);
							mine_k.qkey = KeyBind->key_use_ID[4].get_key(0);
							mine_k.ekey = KeyBind->key_use_ID[5].get_key(0);
							mine_k.reload = KeyBind->key_use_ID[6].get_key(0);
							mine_k.have_item = KeyBind->key_use_ID[7].get_key(2);
							mine_k.drop_ = KeyBind->key_use_ID[8].get_key(2);
							mine_k.squat = KeyBind->key_use_ID[9].get_key(1);
							mine_k.sort_magazine = KeyBind->key_use_ID[12].get_key(2);
							mine_k.running = KeyBind->key_use_ID[13].get_key(0);
							mine_k.jamp = KeyBind->key_use_ID[14].get_key(2);
							mine_k.shoot = KeyBind->mouse_use_ID[0].get_key(3);
							mine_k.select = KeyBind->mouse_use_ID[1].get_key(5);
							mine_k.aim = KeyBind->mouse_use_ID[2].get_key(3);
							mine_k.have_mag = true;
						}
						selend = SELECTscene->UpDate();
						cam_t = &SELECTscene->Get_Camera();
						break;
					case scenes::MAIN_LOOP:
						//
						if (DrawPts->use_vr) {
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
									mine_k.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);	//�}�K�W������
									mine_k.have_item = (ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0;		//�A�C�e���擾
									mine_k.sort_magazine = false;									//
									mine_k.drop_ = false;											//
									mine_k.running = (ptr_->on[0] & BUTTON_TOUCHPAD) != 0;			//running
									mine_k.jamp = (ptr_->on[0] & BUTTON_SIDE) != 0;					//jamp
								}
							}
						}
						else {
							auto& mine_k = MAINLOOPscene->Get_Mine().get_key_();
							//�L��
							KeyBind->key_use_ID[9].on_off.first = mine_k.squat;
							//�ݒ�
							mine_k.wkey = KeyBind->key_use_ID[0].get_key(0);
							mine_k.skey = KeyBind->key_use_ID[1].get_key(0);
							mine_k.dkey = KeyBind->key_use_ID[2].get_key(0);
							mine_k.akey = KeyBind->key_use_ID[3].get_key(0);
							mine_k.qkey = KeyBind->key_use_ID[4].get_key(0);
							mine_k.ekey = KeyBind->key_use_ID[5].get_key(0);
							mine_k.reload = KeyBind->key_use_ID[6].get_key(0);
							mine_k.have_item = KeyBind->key_use_ID[7].get_key(2);
							mine_k.drop_ = KeyBind->key_use_ID[8].get_key(2);
							mine_k.squat = KeyBind->key_use_ID[9].get_key(1);
							mine_k.sort_magazine = KeyBind->key_use_ID[12].get_key(2);
							mine_k.running = KeyBind->key_use_ID[13].get_key(0);
							mine_k.jamp = KeyBind->key_use_ID[14].get_key(0);
							mine_k.TPS = KeyBind->key_use_ID[15].get_key(0);
							mine_k.shoot = KeyBind->mouse_use_ID[0].get_key(3);
							mine_k.select = KeyBind->mouse_use_ID[1].get_key(5);
							mine_k.aim = KeyBind->mouse_use_ID[2].get_key(3);
							mine_k.have_mag = true;
						}
						selend = MAINLOOPscene->UpDate();
						cam_t = &MAINLOOPscene->Get_Camera();
						if (KeyBind->key_use_ID[10].get_key(0)) {
							selend = false;
						}
						break;
					default:
						break;
					}
					//VR��ԂɓK�p
					DrawPts->Move_Player();
					//�`��
					{
						//UI��������
						HostpassPTs->Set_UI_draw([&]() {
							switch (sel_scene) {
							case scenes::ITEM_LOAD:
								UI_LOADPTs->UI_Draw(DrawPts->use_vr);
								break;
							case scenes::MAP_LOAD:
								UI_LOADPTs->UI_Draw(DrawPts->use_vr);
								break;
							case scenes::LOAD:
								LOADscene->UI_Draw();
								break;
							case scenes::SELECT:
								SELECTscene->UI_Draw();
								break;
							case scenes::MAIN_LOOP:
								MAINLOOPscene->UI_Draw();
								break;
							default:
								break;
							}
						});
						if (cam_t != nullptr) {
							//���ʒu�w��
							Set3DSoundListenerPosAndFrontPosAndUpVec(cam_t->campos.get(), cam_t->camvec.get(), cam_t->camup.get());
							//�e�p��
							DrawPts->Ready_Shadow(cam_t->campos, [&] {
								switch (sel_scene) {
								case scenes::ITEM_LOAD:
									break;
								case scenes::MAP_LOAD:
									break;
								case scenes::LOAD:
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
							//��near�͂���
							//	(MAINLOOPscene->Get_Mine().get_alive()) ? VGet(2.f, 2.5f, 2.f) : VGet(10.f, 2.5f, 10.f)
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
										break;
									case scenes::SELECT:
										HostpassPTs->BUF_draw([&](void) { SELECTscene->BG_Draw(); }, [&](void) { DrawPts->Draw_by_Shadow([&] { SELECTscene->Main_Draw(); }); }, tmp_cam);
										break;
									case scenes::MAIN_LOOP:
										HostpassPTs->BUF_draw([&](void) { MAINLOOPscene->BG_Draw(); }, [&](void) { DrawPts->Draw_by_Shadow([&] { MAINLOOPscene->Main_Draw(); }); }, tmp_cam);
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
										break;
									case scenes::MAP_LOAD:
										break;
									case scenes::LOAD:
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
						//�L�[
						KeyBind->draw();
						//�f�o�b�O
						DebugPTs->end_way();
						DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}
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
					MAPPTs->Start(
						MAINLOOPscene->get_parts_data(EnumGunParts::PARTS_MAGAZINE),
						&MAINLOOPscene->get_meds_data(),
						VGet(0.5f, -0.5f, 0.5f)/*MAINLOOPscene->get_Light_vec()*/
					);
					break;
				case scenes::LOAD:
					LOADscene->Dispose(&SELECTscene->preset);
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