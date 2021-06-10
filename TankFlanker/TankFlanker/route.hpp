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
	//�I�������t���O
	bool ending = true;
	int sel_scene = scenes::NONE_SCENE;
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
		//int PixelShaderHandle;
		//int VertexShaderHandle;
		//VertexShaderHandle = LoadVertexShader("shader/NormalMesh_PointLightVS.vso");			// ���_�V�F�[�_�[��ǂݍ���
		//PixelShaderHandle = LoadPixelShader("shader/NormalMesh_PointLightPS.pso");		// �s�N�Z���V�F�[�_�[��ǂݍ���
		//SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
		//SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
		//MV1SetUseOrigShader(TRUE);	// ���f���̕`��ɃI���W�i���V�F�[�_�[���g�p����ݒ���n�m�ɂ���

		int pshandle, vshandle;
		VERTEX3DSHADER Screen_vertex[6] = { 0.0f };
		FLOAT2 *dispsize;
		FLOAT4 *f4;
		int pscbhandle;
		int pscbhandle2;

		vshandle = LoadVertexShader("shader/ShaderPolygon3DTestVS.vso");	// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
		pshandle = LoadPixelShader("shader/ShaderPolygon3DTestPS.pso");		// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���

		// ���_�f�[�^�̏���
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

		auto HostpassPTs = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		auto MAPPTs = std::make_unique<MAPclass::Map>(OPTPTs->grass_level);																	//MAP
		//�L�[�ǂݍ���
		auto KeyBind = std::make_unique<key_bind>();
		auto PauseMenu = std::make_unique<pause_menu>(&KeyBind);
		//�V�[��
		auto UI_LOADPTs = std::make_unique<UIclass::UI_LOADING>();
		auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(&MAPPTs, &OPTPTs);
		auto LOADscene = std::make_shared<Sceneclass::LOAD>();
		auto SELECTscene = std::make_shared<Sceneclass::SELECT>();

		UI_LOADPTs->Get_ptr(&DrawPts, &MAPPTs);
		LOADscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		SELECTscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);
		MAINLOOPscene->Get_ptr(&DrawPts, &OPTPTs, &MAPPTs, &MAINLOOPscene, &DebugPTs);

		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr;

		//�J��Ԃ�
		do {
			{
				//�J��
				//scenes_ptr��null�̏ꍇ���[�h���
				scenes_ptr = nullptr;
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					MAINLOOPscene->Start_After();
					sel_scene = scenes::MAP_LOAD;
					break;
				case scenes::MAP_LOAD:
					MAPPTs->Start();
					MAINLOOPscene->Set_Charaa(MAPPTs->get_spawn_point().size());	//�L�����ݒ�
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
					sel_scene = scenes::MAP_LOAD;
					break;
				default:
					sel_scene = scenes::ITEM_LOAD;
					break;
				}
			}
			//�J�n
			{
				//
				if (scenes_ptr != nullptr) {
					scenes_ptr->Set();
					DrawPts->Set_Light_Shadow(scenes_ptr->get_Shadow_maxpos(), scenes_ptr->get_Shadow_minpos(), scenes_ptr->get_Light_vec(), [&] {scenes_ptr->Shadow_Draw_Far(); });
					SetGlobalAmbientLight(scenes_ptr->get_Light_color());
				}
				switch (sel_scene) {
				case scenes::ITEM_LOAD:
					UI_LOADPTs->Set("�A�C�e���f�[�^");					//�A�C�e���ǂݍ���
					break;
				case scenes::MAP_LOAD:
					MAPPTs->Ready_map("data/map_new2");								//�}�b�v�ǂݍ���
					UI_LOADPTs->Set("�}�b�v");							//�}�b�v�ǂݍ���
					break;
				case scenes::MAIN_LOOP:
					MAPPTs->Start_Ray(scenes_ptr->get_Light_vec());
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
				auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
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
					selpause = false;
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::MAP_LOAD:
						selend = UI_LOADPTs->Update();
						break;
					case scenes::LOAD:
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
								//�ݒ�
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
							}
						}
						break;
					case scenes::SELECT:
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
								//�ݒ�
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
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = (*MAINLOOPscene->Get_Mine())->get_key_();
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
						}
						else {
							//�L�[�A�N�e�B�u
							PauseMenu->Pause_key_active();
							//
							selend = PauseMenu->Update();
						}
						break;
					default:
						break;
					}
					if (scenes_ptr != nullptr) {
						if (!selpause) {
							selend = scenes_ptr->UpDate();
						}
						cam_t = &scenes_ptr->Get_Camera();
					}
					//VR��ԂɓK�p
					DrawPts->Move_Player();
					//�`��
					{
						//UI��������
						HostpassPTs->Set_UI_draw([&](void) noexcept {
							if (scenes_ptr != nullptr) {
								scenes_ptr->UI_Draw();
							}
							else {
								UI_LOADPTs->UI_Draw();
							}
						});
						if (cam_t != nullptr) {
							//���ʒu�w��
							Set3DSoundListenerPosAndFrontPosAndUpVec(cam_t->campos.get(), cam_t->camvec.get(), cam_t->camup.get());
							//�e�p��
							if (scenes_ptr != nullptr) {
								DrawPts->Ready_Shadow(cam_t->campos, [&] { scenes_ptr->Shadow_Draw(); }, [&] { scenes_ptr->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));
							}
							//scenes::MAIN_LOOP:
							//��near�͂���
							//	(scenes_ptr->Get_Mine()->get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
									if (scenes_ptr != nullptr) {
										HostpassPTs->BUF_draw([&](void) noexcept { scenes_ptr->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, update_effect_f);
									}
									//�ŏI�`��
									HostpassPTs->Set_MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp);
								{
									bool lens = false;
									float zoom = 1.f;
									float reticle_size = 1.f;
									//��ʑ̐[�x�`��
									if (scenes_ptr != nullptr) {
										lens = scenes_ptr->is_lens();
										zoom = scenes_ptr->zoom_lens();
										reticle_size = scenes_ptr->size_lens();
									}
									if (!lens) {
										//�f�t�H�`��
										HostpassPTs->MAIN_draw();
									}
									else {
										//�����Y�`��
										SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());		// �g�p����e�N�X�`�����Z�b�g
										dispsize = (FLOAT2 *)GetBufferShaderConstantBuffer(pscbhandle);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
										dispsize->u = float(DrawPts->disp_x);
										dispsize->v = float(DrawPts->disp_y);
										UpdateShaderConstantBuffer(pscbhandle);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
										SetShaderConstantBuffer(pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
										f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(pscbhandle2);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
										f4->x = float(DrawPts->disp_x) / 2.f;
										f4->y = float(DrawPts->disp_y) / 2.f;
										f4->z = reticle_size;
										f4->w = zoom;
										UpdateShaderConstantBuffer(pscbhandle2);							// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
										SetShaderConstantBuffer(pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
										SetUseVertexShader(vshandle);		// �g�p���钸�_�V�F�[�_�[���Z�b�g
										SetUsePixelShader(pshandle);		// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
										MV1SetUseOrigShader(TRUE);
										DrawPolygon3DToShader(Screen_vertex, 2);		// �`��
										MV1SetUseOrigShader(FALSE);
									}
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									HostpassPTs->Draw(cam_t, DrawPts->use_vr);
									//UI2
									if (scenes_ptr != nullptr) {
										scenes_ptr->Item_Draw();
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
						if (scenes_ptr != nullptr) {
							scenes_ptr->LAST_Draw();
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
				//���
				if (scenes_ptr != nullptr) {
					scenes_ptr->Dispose();
				}
				else {
					UI_LOADPTs->Dispose();
				}
				DrawPts->Delete_Shadow();
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