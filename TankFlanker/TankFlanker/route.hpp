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
	public:
		main_c(void) noexcept {
			//�V�F�[�_�[
			shaders::shader_Vertex Screen_vertex;					// ���_�f�[�^
			std::array<shaders, 2> shader2D;

			OPTION::Create();
			auto* OptionParts = OPTION::Instance();
			OptionParts->Load();								//�ݒ�ǂݍ���
			DXDraw::Create("FPS_n2", Frame_Rate);			//�ėp
#ifdef DEBUG
			DebugClass::Create(Frame_Rate);
			auto DebugParts = DebugClass::Instance();					//�f�o�b�O
#endif // DEBUG
			auto* DrawParts = DXDraw::Instance();
			OptionParts->Set_useVR(DrawParts->use_vr);
			//�V�F�[�_�[
			PostPassEffect::Create();
			auto PostPassParts = PostPassEffect::Instance();				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
			//�V�F�[�_�[
			Screen_vertex.Set();																					// ���_�f�[�^�̏���
			shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
			shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");									//�c��
			//MAP
			auto MAPPTs = std::make_shared<MAPclass::Map>(OptionParts->Get_grass_level(), DrawParts->disp_x, DrawParts->disp_y);
			//�L�[�ǂݍ���
			auto KeyBind = std::make_shared<key_bind>();
			auto OptionMenu = std::make_shared<option_menu>(KeyBind);
			auto PauseMenu = std::make_unique<pause_menu>(OptionMenu, KeyBind);
			//���\�[�X
			auto Audio_resource = std::make_shared<Audio_Control>();													//�I�[�f�B�I
			auto GunPartses = std::make_shared<GunPartsControl>();														//�e�p�[�c
			effectControl.Init();																						//�G�t�F�N�g
			OptionMenu->Set();
			hit_obj_p.Init();																							//�e��
			hit_b_obj_p.Init();																							//����
			//�V�[��
			auto ITEMLOADscene = std::make_shared<Sceneclass::LOADING>(MAPPTs, GunPartses, KeyBind);
			auto MAPLOADscene = std::make_shared<Sceneclass::LOADING>(MAPPTs, GunPartses, KeyBind);
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(MAPPTs, GunPartses, KeyBind);
			auto LOADscene = std::make_shared<Sceneclass::LOAD>(MAPPTs, GunPartses, KeyBind);
			auto SELECTscene = std::make_shared<Sceneclass::SELECT>(MAPPTs, GunPartses, KeyBind);
			//�J�n����
			ITEMLOADscene->Awake();
			MAPLOADscene->Awake();
			LOADscene->Awake();
			SELECTscene->Awake();
			MAINLOOPscene->Awake();
			//�J�ڐ�w��
			ITEMLOADscene->Set_Next(MAPLOADscene, scenes::MAP_LOAD);
			MAPLOADscene->Set_Next(LOADscene, scenes::LOAD);
			LOADscene->Set_Next(SELECTscene, scenes::SELECT);
			SELECTscene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			MAINLOOPscene->Set_Next(MAPLOADscene, scenes::MAP_LOAD);
			//�ǂݍ��ݕ\���e�L�X�g�w��
			ITEMLOADscene->settitle("�A�C�e���f�[�^");		//�A�C�e���ǂݍ���
			MAPLOADscene->settitle("�}�b�v");				//�}�b�v�ǂݍ���
			//�J�n���J�ڐ�
			sel_scene = scenes::ITEM_LOAD;
			scenes_ptr = ITEMLOADscene;
			//�J��Ԃ�
			do {
				//�J�n
				scenes_ptr->Set();
				selend = true;
				selpause = false;
				//
				while (ProcessMessage() == 0) {
#ifdef DEBUG
					clsDx();
#endif // DEBUG
					const auto waits = GetNowHiPerformanceCount();
					FPS = GetFPS();
#ifdef DEBUG
					DebugParts->put_way();
#endif // DEBUG
					{
						//�X�V
						{
							KeyBind->ReSet_isalways();
							selpause = false;
							if (sel_scene == scenes::MAIN_LOOP) {
								selpause = PauseMenu->Pause_key();
							}
							if (!selpause) {
								if (DrawParts->use_vr) {
									scenes_ptr->KeyOperation_VR();
								}
								else {
									scenes_ptr->KeyOperation();
								}
							}
							if (selpause) {
								OptionMenu->Update();
							}
							selend = (!selpause) ? scenes_ptr->Update() : PauseMenu->Update();

							//�e���̍X�V
							hit_obj_p.Update();
							hit_b_obj_p.Update();
							//���C�g�̍X�V
							Light_pool.Update(scenes_ptr->Get_Camera().campos);
						}
						//VR��ԂɓK�p
						DrawParts->Move_Player();
						//�`��
						{
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							if (!selpause) {
								effectControl.Calc();
							}
							//���ʂ̕`��O�p��
							scenes_ptr->ReadyDraw();
							//UI��������
							PostPassParts->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VR�Ɉڂ�
							DrawParts->Draw([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									PostPassParts->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawParts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//�ŏI�`��
									PostPassParts->Set_MAIN_Draw();
								}
								//*
								GraphHandle::SetDraw_Screen(tmp);
								{
									SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
									if (scenes_ptr->is_lens()) {
										//�����Y�`��
										shader2D[0].Set_dispsize();
										shader2D[0].Set_param(float(DrawParts->disp_x) / 2.f, float(DrawParts->disp_y) / 2.f, scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
										PostPassParts->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[0].Draw(Screen_vertex);
										}
										PostPassParts->Set_MAIN_Draw_nohost();
									}

									if (scenes_ptr->is_bless()) {
										//�c�ݕ`��
										shader2D[1].Set_dispsize();
										shader2D[1].Set_param(0, 0, scenes_ptr->ratio_bless(), (1.f - cos(scenes_ptr->time_bless())) / 2.f);
										PostPassParts->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[1].Draw(Screen_vertex);
										}
										PostPassParts->Set_MAIN_Draw_nohost();
									}
									SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
								}
								//*/
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									PostPassParts->MAIN_Draw();											//�f�t�H�`��
									PostPassParts->DrawUI(&scenes_ptr->Get_Camera(), DrawParts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
							//�`��
							if (DrawParts->use_vr) {
								DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawParts->outScreen[0].DrawRotaGraph(DrawParts->disp_x / 2, DrawParts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawParts->outScreen[0].DrawGraph(0, 0, false);
							}
							//��ɏ���
							scenes_ptr->LAST_Draw();
							//�|�[�Y
							PauseMenu->Draw();
							OptionMenu->Draw();
							//�L�[
							KeyBind->Draw();
							//�f�o�b�O
#ifdef DEBUG
							DebugParts->end_way();
							DebugParts->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
#endif // DEBUG
						}
					}
					//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
#ifdef DEBUG
					printfDx("call  :%d\n", GetDrawCallCount());
					printfDx("Async :%d\n", GetASyncLoadNum());
#endif // DEBUG
					//��ʂ̔��f
					DrawParts->Screen_Flip();
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
						GunPartses->Set();												//
						MAINLOOPscene->Start();											//���C�����[�v�J�n�ǂݍ���
						hit_obj_p.Clear();												//�e��
						hit_b_obj_p.Clear();											//�e��
						break;
					case scenes::MAP_LOAD:
						MAPPTs->Start();												//�}�b�v�p�[�c����
						MAINLOOPscene->Ready_Chara(MAPPTs->Get_spawn_point().size());	//�L�����ݒ�
						MAINLOOPscene->Ready_Tank(MAPPTs->Get_spawn_point().size());	//��Ԏw��
						break;
					case scenes::LOAD:
						SELECTscene->Start(LOADscene->putout_Preset());					//�v���Z�b�g���w��
						break;
					case scenes::MAIN_LOOP:
						MAPPTs->Dispose();												//�}�b�v������
						break;
					}
					//�J��
					{
						sel_scene = scenes_ptr->Next_scene;
						scenes_ptr = scenes_ptr->Next_ptr;
					}
					if (sel_scene == scenes::MAP_LOAD) { MAPPTs->Ready_map("data/map"); }
				}
			} while (this->ending);
			effectControl.Dispose();
		}
	};
};