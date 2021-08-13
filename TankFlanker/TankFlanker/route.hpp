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
		shaders::shader_Vertex Screen_vertex;					// ���_�f�[�^
		std::array<shaders, 2> shader2D;
	public:
		main_c(void) noexcept {
			auto OPTPTs = std::make_shared<OPTION>();								//�ݒ�ǂݍ���
			auto DrawPts = std::make_shared<DXDraw>("FPS_n2", OPTPTs, Frame_Rate);		//�ėp
#ifdef DEBUG
			auto DebugPTs = std::make_shared<DeBuG>(Frame_Rate);					//�f�o�b�O
#endif // DEBUG
			OPTPTs->Set_useVR(DrawPts->use_vr);
			//�G�t�F�N�g
			effectControl.Init();
			//�V�F�[�_�[
			auto HostpassPTs = std::make_shared<HostPassEffect>(OPTPTs, DrawPts->disp_x, DrawPts->disp_y);				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
			//�V�F�[�_�[
			Screen_vertex.Set(DrawPts);			// ���_�f�[�^�̏���
			shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
			shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");									//�c��
			//MAP
			auto MAPPTs = std::make_shared<MAPclass::Map>(OPTPTs->Get_grass_level(), DrawPts->disp_x, DrawPts->disp_y);
			MAPPTs->Set_mine(MAPPTs);
			//�L�[�ǂݍ���
			auto KeyBind = std::make_shared<key_bind>(DrawPts);
			auto PauseMenu = std::make_unique<pause_menu>(KeyBind, DrawPts);
			//�V�[��
			auto UI_LOADPTs = std::make_shared<Sceneclass::LOADING>();
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(MAPPTs, OPTPTs);
			auto LOADscene = std::make_shared<Sceneclass::LOAD>();
			auto SELECTscene = std::make_shared<Sceneclass::SELECT>();
			//���\�[�X
			auto Audio_resource = std::make_shared<Audio_Control>();						//�I�[�f�B�I
			auto GunPartses = std::make_shared<GUNPARTS_Control>();							//�e�p�[�c
			effectControl.Start();
			//
			UI_LOADPTs->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			LOADscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			SELECTscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
			MAINLOOPscene->Init(DrawPts, OPTPTs, MAPPTs, GunPartses, KeyBind);
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
						//�X�V
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
						//VR��ԂɓK�p
						DrawPts->Move_Player();
						//�`��
						{
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							if (!selpause) {
								effectControl.Calc();
							}
							//���ʂ̕`��O�p��
							scenes_ptr->ReadyDraw();
							//UI��������
							HostpassPTs->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VR�Ɉڂ�
							DrawPts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									HostpassPTs->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//�ŏI�`��
									HostpassPTs->Set_MAIN_Draw();
								}
								//*
								GraphHandle::SetDraw_Screen(tmp);
								{
									SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
									if (scenes_ptr->is_lens()) {
										//�����Y�`��
										shader2D[0].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[0].set_param(float(DrawPts->disp_x) / 2.f, float(DrawPts->disp_y) / 2.f, scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[0].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}

									if (scenes_ptr->is_bless()) {
										//�c�ݕ`��
										shader2D[1].set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[1].set_param(0, 0, scenes_ptr->ratio_bless(), (1.f - cos(scenes_ptr->time_bless())) / 2.f);
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[1].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}
									SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
								}
								//*/
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									HostpassPTs->MAIN_Draw();											//�f�t�H�`��
									HostpassPTs->DrawUI(&scenes_ptr->Get_Camera(), DrawPts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
							//�`��
							if (DrawPts->use_vr) {
								DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawPts->outScreen[0].DrawRotaGraph(DrawPts->disp_x / 2, DrawPts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawPts->outScreen[0].DrawGraph(0, 0, false);
							}
							//��ɏ���
							scenes_ptr->LAST_Draw();
							//�|�[�Y
							PauseMenu->Draw();
							//�L�[
							KeyBind->Draw();
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
						GunPartses->Set();																//
						MAINLOOPscene->Start();															//���C�����[�v�J�n�ǂݍ���
						break;
					case scenes::MAP_LOAD:
						MAPPTs->Start();																//�}�b�v�p�[�c����
						MAINLOOPscene->Ready_Chara(MAPPTs->Get_spawn_point().size());					//�L�����ݒ�
						LOADscene->Start(MAINLOOPscene->Get_Mine());									//
						break;
					case scenes::LOAD:
						SELECTscene->Start(LOADscene->putout_Preset(), MAINLOOPscene->Get_Mine());		//�v���Z�b�g���w��
						break;
					case scenes::SELECT:
						MAINLOOPscene->Ready_Tank(2);													//��Ԏw��
						break;
					case scenes::MAIN_LOOP:
						MAPPTs->Dispose();																//�}�b�v������
						break;
					}
				}
				//
			} while (this->ending);
			//
			//MAINLOOPscene->Dispose();//���
			MAPPTs->Dispose();
			DrawPts->Delete_Shadow();
			effectControl.Dispose();
		}
	};
};