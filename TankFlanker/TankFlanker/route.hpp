#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class main_c {
	private:
		bool ending{ true };							//�I�������t���O
		scenes sel_scene{ scenes::NONE_SCENE };			//���݂̃V�[��
		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
		bool selend{ true };
		bool selpause{ true };
		//�V�F�[�_�[
		shader_Vertex Screen_vertex;					// ���_�f�[�^
		std::array<shaders, 2> shader2D;
		//�G�t�F�N�g
		EffectControl effectControl;
	public:
		main_c(void) noexcept {
			auto OPTPTs = std::make_shared<OPTION>();								//�ݒ�ǂݍ���
			auto DrawPts = std::make_shared<DXDraw>("FPS_n2", OPTPTs, Frame_Rate);	//�ėp
#ifdef DEBUG
			auto DebugPTs = std::make_shared<DeBuG>(Frame_Rate);					//�f�o�b�O
#endif // DEBUG
			OPTPTs->Set_useVR(DrawPts->use_vr);
			Screen_vertex.Set();			// ���_�f�[�^�̏���
			//�G�t�F�N�g
			effectControl.Init();
			//�V�F�[�_�[
			auto HostpassPTs = std::make_shared<HostPassEffect>(OPTPTs, DrawPts->disp_x, DrawPts->disp_y);				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
			shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
			shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");									//�c��
			//MAP
			auto MAPPTs = std::make_shared<MAPclass::Map>(OPTPTs->Get_grass_level(), DrawPts->disp_x, DrawPts->disp_y);
			MAPPTs->Set_mine(MAPPTs);
			//�L�[�ǂݍ���
			auto KeyBind = std::make_shared<key_bind>();
			auto PauseMenu = std::make_shared<pause_menu>(KeyBind);
			//�V�[��
			auto UI_LOADPTs = std::make_shared<Sceneclass::LOADING>();
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(MAPPTs, OPTPTs);
			auto LOADscene = std::make_shared<Sceneclass::LOAD>();
			auto SELECTscene = std::make_shared<Sceneclass::SELECT>();
			//
			UI_LOADPTs->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, &effectControl.effsorce);
			LOADscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, &effectControl.effsorce);
			SELECTscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, &effectControl.effsorce);
			MAINLOOPscene->Init(DrawPts, OPTPTs, MAPPTs, MAINLOOPscene, &effectControl.effsorce);
			//�J�ڐ�w��
			LOADscene->Set_Next(SELECTscene, scenes::SELECT);
			SELECTscene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			MAINLOOPscene->Set_Next(UI_LOADPTs, scenes::MAP_LOAD);
			//�J��Ԃ�
			do {
				//�J��
				{
					//�J�ڑO����
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						scenes_ptr->Set_Next(UI_LOADPTs, scenes::MAP_LOAD);
						break;
					case scenes::MAP_LOAD:
						scenes_ptr->Set_Next(LOADscene, scenes::LOAD);
						break;
					default: break;
					}
					//�J�ڏ���
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
					//�J�ڌ㏈��
					switch (sel_scene) {
					case scenes::ITEM_LOAD:
						UI_LOADPTs->settitle("�A�C�e���f�[�^");	//�A�C�e���ǂݍ���
						break;
					case scenes::MAP_LOAD:
						UI_LOADPTs->settitle("�}�b�v");				//�}�b�v�ǂݍ���
						MAPPTs->Ready_map("data/map");				//�}�b�v�ǂݍ���
						break;
					default: break;
					}
				}
				//�J�n
				{
					scenes_ptr->Set();
					if (sel_scene == scenes::MAIN_LOOP) {
						MAPPTs->Start_Ray(scenes_ptr->get_Light_vec());
					}
					selend = true;
					selpause = false;
					effectControl.Start();
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
						//�X�V
						KeyBind->reSet_isalways();
						selpause = false;
						if (sel_scene == scenes::MAIN_LOOP) {
							selpause = PauseMenu->Pause_key();
						}
						if (!selpause) {
							if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
								if (MAINLOOPscene->Get_Mine() != nullptr) {
									auto& mine_k = MAINLOOPscene->Get_Mine()->set_key_();
									if (DrawPts->get_hand1_num() != -1) {
										auto ptr_ = DrawPts->get_device_hand1();
										if (ptr_->turn && ptr_->now) {
											mine_k.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//�ˌ�
											mine_k.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																						//�}�O�L���b�`
											mine_k.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);	//�Z���N�^�[
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
								}
#endif // _USE_OPENVR_
							}
							else {
								scenes_ptr->KeyOperation(KeyBind);
							}
						}
						selend = (!selpause) ? scenes_ptr->UpDate() : PauseMenu->Update();
						//VR��ԂɓK�p
						DrawPts->Move_Player();
						//�`��
						{
							//UI��������
							HostpassPTs->Set_UI_draw([&](void) noexcept { scenes_ptr->UI_Draw(); });
							scenes_ptr->ReadyDraw();
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							if (!selpause) {
								effectControl.Calc();
							}
							//VR�Ɉڂ�
							DrawPts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									HostpassPTs->BUF_draw([&](void) noexcept { scenes_ptr->BG_Draw(); }, [&](void) noexcept { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.update_effect_f);
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
					//�����[�v�̏���
					switch (sel_scene) {
					default: break;
					case scenes::ITEM_LOAD:
						MAINLOOPscene->Start();											//���C�����[�v�J�n�ǂݍ���
						break;
					case scenes::MAP_LOAD:
						MAPPTs->Start();												//�}�b�v�p�[�c����
						MAINLOOPscene->Ready_Chara(MAPPTs->get_spawn_point().size());	//�L�����ݒ�
						break;
					case scenes::LOAD:
						SELECTscene->Start(LOADscene->putout_preset());					//�v���Z�b�g���w��
						break;
					case scenes::SELECT:
						MAINLOOPscene->Ready_Tank();									//��Ԏw��
						break;
					case scenes::MAIN_LOOP:
						MAPPTs->Dispose();												//�}�b�v������
						break;
					}
				}
				//
				if (!this->ending) {
					//MAINLOOPscene->Dispose();//���
					MAPPTs->Dispose();
					break;
				}
				//
			} while (true);
			//
			DrawPts->Delete_Shadow();
		}
	};
};