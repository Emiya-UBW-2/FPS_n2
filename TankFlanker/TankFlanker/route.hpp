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
	bool ending{ true };
	scenes sel_scene = NONE_SCENE;
	bool update_effect_f{ true };
	LONGLONG update_effect_was = 0;

	bool selend{ true };
	bool selpause{ true };

	VERTEX3DSHADER Screen_vertex[6] = { 0.0f };

	std::array<shaders2D, 2> shader2D;

	std::vector<EffekseerEffectHandle> effsorce; /*�G�t�F�N�g���\�[�X*/

	std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
public:
	main_c(void) noexcept {
		auto OPTPTs = std::make_shared<OPTION>();																		//�ݒ�ǂݍ���
		auto DrawPts = std::make_shared<DXDraw>("FPS_n2", FRAME_RATE, OPTPTs->useVR, OPTPTs->Shadow, OPTPTs->Vsync);	//�ėp
		auto DebugPTs = std::make_shared<DeBuG>(FRAME_RATE);															//�f�o�b�O
		if (DrawPts->use_vr) {
			//VR�ł͎g���Ȃ��@�\
			OPTPTs->DoF = false;
			OPTPTs->Bloom = false;
			OPTPTs->SSAO = false;
		}
		// ���_�f�[�^�̏���
		{
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
		}
		//�G�t�F�N�g
		{
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind = FindFirstFile("data/effect/*", &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					{
						p = win32fdt.cFileName;
						if (p.find(".efk") != std::string::npos) {
							effsorce.resize(effsorce.size() + 1);
							effsorce.back() = EffekseerEffectHandle::load("data/effect/" + p);
						}
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
		//�V�F�[�_�[
		auto HostpassPTs = std::make_shared<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, DrawPts->disp_x, DrawPts->disp_y);	//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																						//�����Y
		shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");															//�c��
		//MAP
		auto MAPPTs = std::make_shared<MAPclass::Map>(OPTPTs->grass_level);
		//�L�[�ǂݍ���
		auto KeyBind = std::make_shared<key_bind>();
		auto PauseMenu = std::make_shared<pause_menu>(KeyBind);
		//�V�[��
		auto UI_LOADPTs = std::make_shared<Sceneclass::LOADING>();
		auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(MAPPTs, OPTPTs);
		auto LOADscene = std::make_shared<Sceneclass::LOAD>();
		auto SELECTscene = std::make_shared<Sceneclass::SELECT>();
		//
		UI_LOADPTs->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, DebugPTs,&effsorce);
		LOADscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, DebugPTs, &effsorce);
		SELECTscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, DebugPTs, &effsorce);
		MAINLOOPscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, DebugPTs, &effsorce);
		//�J��Ԃ�
		do {
			//�J��
			switch (sel_scene) {
			default:
				UI_LOADPTs->settitle("�A�C�e���f�[�^");							//�A�C�e���ǂݍ���
				sel_scene = ITEM_LOAD;
				scenes_ptr = UI_LOADPTs;
				break;
			case ITEM_LOAD:
				MAINLOOPscene->Start();
				sel_scene = MAP_LOAD;
				scenes_ptr = UI_LOADPTs;
				break;
			case MAP_LOAD:
				MAPPTs->Start(MAINLOOPscene->world);
				MAINLOOPscene->Ready_Chara(MAPPTs->get_spawn_point().size());	//�L�����ݒ�
				sel_scene = LOAD;
				scenes_ptr = LOADscene;
				break;
			case LOAD:
				SELECTscene->preset = LOADscene->putout_preset();
				sel_scene = SELECT;
				scenes_ptr = SELECTscene;
				break;
			case SELECT:
				sel_scene = MAIN_LOOP;
				scenes_ptr = MAINLOOPscene;
				break;
			case MAIN_LOOP:
				MAPPTs->Dispose();
				sel_scene = MAP_LOAD;
				scenes_ptr = UI_LOADPTs;
				break;
			}
			//�J�n
			{
				if (sel_scene == MAP_LOAD) {
					MAPPTs->Ready_map("data/map", DrawPts->disp_x, DrawPts->disp_y);//�}�b�v�ǂݍ���
					UI_LOADPTs->settitle("�}�b�v");									//�}�b�v�ǂݍ���
				}
				scenes_ptr->Set();
				DrawPts->Set_Light_Shadow(scenes_ptr->get_Shadow_maxpos(), scenes_ptr->get_Shadow_minpos(), scenes_ptr->get_Light_vec(), [&] {scenes_ptr->Shadow_Draw_Far(); });
				SetGlobalAmbientLight(scenes_ptr->get_Light_color());
				if (sel_scene == MAIN_LOOP) { MAPPTs->Start_Ray(scenes_ptr->get_Light_vec()); }
				selend = true;
				selpause = false;
				update_effect_was = GetNowHiPerformanceCount();
			}
			//��(���ʂ̂���)
			auto set_key_vr = [&]() {
#ifdef _USE_OPENVR_
				auto& mine_k = MAINLOOPscene->Get_Mine()->get_key_();
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
						mine_k.have_item = ((ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0);	//�A�C�e���擾
						mine_k.sort_magazine = false;									//
						mine_k.view_gun = false;										//
						mine_k.drop_ = false;											//
						mine_k.throw_gre = false;										//
						mine_k.running = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0);		//running
						mine_k.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);				//jamp
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
					switch (sel_scene) {
					case LOAD:
						selpause = false;
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine()->get_key_();
								//�ݒ�
								mine_k.dkey = KeyBind->get_key_use(2);
								mine_k.akey = KeyBind->get_key_use(3);
								mine_k.jamp = KeyBind->get_key_use(14);
							}
						}
						break;
					case SELECT:
						selpause = false;
						if (!selpause) {
							//�L�[�A�N�e�B�u
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine()->get_key_();
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
					case MAIN_LOOP:
						selpause = PauseMenu->Pause_key();
						//selpause ^= 1;
						if (!selpause) {
							if (DrawPts->use_vr) {
								set_key_vr();
							}
							else {
								auto& mine_k = MAINLOOPscene->Get_Mine()->get_key_();
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
						break;
					default:
						break;
					}
					selend = (!selpause) ? scenes_ptr->UpDate() : PauseMenu->Update();
					//VR��ԂɓK�p
					DrawPts->Move_Player();
					//�`��
					{
						//UI��������
						HostpassPTs->Set_UI_draw([&](void) noexcept { scenes_ptr->UI_Draw(); });
						//���ʒu�w��
						Set3DSoundListenerPosAndFrontPosAndUpVec(scenes_ptr->Get_Camera().campos.get(), scenes_ptr->Get_Camera().camvec.get(), scenes_ptr->Get_Camera().camup.get());
						//�e�p��
						DrawPts->Ready_Shadow(scenes_ptr->Get_Camera().campos, [&] { scenes_ptr->Shadow_Draw(); }, [&] { scenes_ptr->Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));//MAIN_LOOP��near�͂��� (scenes_ptr->Get_Mine()->get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
						//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
						{
							update_effect_f = (!selpause && (GetNowHiPerformanceCount() - update_effect_was) >= 1000000 / 60);
							if (update_effect_f) {
								update_effect_was = GetNowHiPerformanceCount();
							}
						}
						//VR�Ɉڂ�
						DrawPts->draw_VR([&] {
							auto tmp = GetDrawScreen();
							auto tmp_cam = scenes_ptr->Get_Camera();
							tmp_cam.campos = GetCameraPosition();
							tmp_cam.camvec = GetCameraTarget();
							{
								//��ʑ̐[�x�`��
								HostpassPTs->BUF_draw([&](void) noexcept { scenes_ptr->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, update_effect_f);
								//�ŏI�`��
								HostpassPTs->Set_MAIN_draw();
							}
							GraphHandle::SetDraw_Screen(tmp);
							{
								SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
								if (scenes_ptr->is_lens()) {
									//�����Y�`��
									shader2D[0].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
									shader2D[0].set_param(float(DrawPts->disp_x) / 2.f, float(DrawPts->disp_y) / 2.f, scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
									HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
									{
										shader2D[0].draw(Screen_vertex);
									}
									HostpassPTs->Set_MAIN_draw_nohost();
								}

								if (scenes_ptr->is_bless()) {
									//�c�ݕ`��
									shader2D[1].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
									shader2D[1].set_param(0, 0, scenes_ptr->ratio_bless(), (1.f - cos(scenes_ptr->time_bless())) / 2.f);
									HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
									{
										shader2D[1].draw(Screen_vertex);
									}
									HostpassPTs->Set_MAIN_draw_nohost();
								}
								SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
							}
							GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
							{
								HostpassPTs->MAIN_draw();						//�f�t�H�`��
								HostpassPTs->DrawUI(&scenes_ptr->Get_Camera(), DrawPts->use_vr);	//UI1
								scenes_ptr->Item_Draw();						//UI2
							}
						}, scenes_ptr->Get_Camera());
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
						scenes_ptr->LAST_Draw();
						//�|�[�Y
						PauseMenu->draw();
						//�L�[
						KeyBind->draw();
						//�f�o�b�O
						DebugPTs->end_way();
						DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					}
				}
				//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);

				printfDx("call  :%d\n", GetDrawCallCount());
				printfDx("Async :%d\n", GetASyncLoadNum());
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
				scenes_ptr->Dispose();
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