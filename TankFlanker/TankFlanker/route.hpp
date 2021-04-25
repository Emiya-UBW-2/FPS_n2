#pragma once
class main_c : Mainclass {
	//�J����
	cam_info camera_main;
	//�I�������t���O
	bool ending = true;
	//�f�[�^
	GraphHandle UI_Screen;						//�`��X�N���[��
	MV1 body_obj, body_col;						//�g�̃��f��
	MV1 hit_pic;								//�e��
	GraphHandle light;							//���C�g�e�N�X�`��
	std::vector<gunparts> mazzule_data;			//GUN�f�[�^
	std::vector<gunparts> grip_data;			//GUN�f�[�^
	std::vector<gunparts> uperhandguard_data;	//GUN�f�[�^
	std::vector<gunparts> underhandguard_data;	//GUN�f�[�^
	std::vector<gunparts> stock_data;			//GUN�f�[�^
	std::vector<gunparts> dustcover_data;		//GUN�f�[�^
	std::vector<gunparts> sight_data;			//GUN�f�[�^
	std::vector<gunparts> foregrip_data;		//GUN�f�[�^
	std::vector<gunparts> light_data;			//GUN�f�[�^
	std::vector<gunparts> laser_data;			//GUN�f�[�^
	std::vector<gunparts> mount_data;			//GUN�f�[�^
	std::vector<gunparts> gun_data;				//GUN�f�[�^
	std::vector<gunparts> mag_data;				//GUN�f�[�^
	std::vector<Meds> meds_data;				//GUN�f�[�^
	//�I�u�W�F
	std::vector<Chara> chara;					//�L����
	std::vector<Items> item;					//�A�C�e��
	std::vector<Hit> hit_obj;					//�e��
	size_t hit_buf = 0;							//
	//
public:
	main_c() {
		auto Optionparts = std::make_unique<OPTION>();																//�ݒ�ǂݍ���
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE,
			Optionparts->useVR_e, Optionparts->shadow_e, Optionparts->vsync_e);										//�ėp
		auto UIparts = std::make_unique<UI>();																		//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);														//�f�o�b�O
		auto Hostpassparts = std::make_unique<HostPassEffect>(
			Optionparts->dof_e, Optionparts->bloom_e, Drawparts->disp_x, Drawparts->disp_y);						//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		UI_Screen = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y, true);									//VR�A�t���X�N���[�����pUI
		auto Hostpassparts_TPS = std::make_unique<HostPassEffect>(Optionparts->dof_e, Optionparts->bloom_e, 
			deskx, desky);																							//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[�������ATPS�p)
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		auto minimapparts = std::make_unique<Minimapclass>();														//MINIMAP
		auto ruleparts = std::make_unique<rule>();																	//���[��
		auto TPSparts = std::make_unique<TPS_parts>();																//3�l��
		auto font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
		//model
		light = GraphHandle::Load("data/light.png");					//���C�g
		MV1::Load("data/model/body2/model.mv1", &this->body_obj, true);	//�g��
		MV1::Load("data/model/body2/col.mv1", &this->body_col, true);	//�g��col
		MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);	//�e��
		//�~�j�}�b�v
		minimapparts->load();
		//�p�[�c�f�[�^
		this->mazzule_data.resize(2);
		this->mazzule_data[0].mod.Ready("data/parts/mazzule/", "AK_6P1_Compensator");
		this->mazzule_data[1].mod.Ready("data/parts/mazzule/", "sup");
		this->grip_data.resize(3);
		this->grip_data[0].mod.Ready("data/parts/grip/", "AK_6P1_Wood_Grip");
		this->grip_data[1].mod.Ready("data/parts/grip/", "AK_original_Grip");
		this->grip_data[2].mod.Ready("data/parts/grip/", "AK_Zenit_RK3_Grip");
		this->uperhandguard_data.resize(4);
		this->uperhandguard_data[0].mod.Ready("data/parts/uper_handguard/", "AK_6P1_Uper_Handguard");
		this->uperhandguard_data[1].mod.Ready("data/parts/uper_handguard/", "AK_Original_Polymer_Uper_Handguard");
		this->uperhandguard_data[2].mod.Ready("data/parts/uper_handguard/", "AK_Steel_Uper_Handguard");
		this->uperhandguard_data[3].mod.Ready("data/parts/uper_handguard/", "AK_Rail_Band");
		this->underhandguard_data.resize(3);
		this->underhandguard_data[0].mod.Ready("data/parts/under_handguard/", "AK_6P1_Under_Handguard");
		this->underhandguard_data[1].mod.Ready("data/parts/under_handguard/", "AK_Original_Polymer_Under_Handguard");
		this->underhandguard_data[2].mod.Ready("data/parts/under_handguard/", "AK_Zenit_B10M_Under_Handguard");
		this->mount_data.resize(2);
		this->mount_data[0].mod.Ready("data/parts/mount/", "AK_Dovetail_Mount");
		this->mount_data[1].mod.Ready("data/parts/mount/", "AK_SideRailMount");
		this->stock_data.resize(1);
		this->stock_data[0].mod.Ready("data/parts/stock/", "AK_6P1_Stock");
		this->dustcover_data.resize(1);
		this->dustcover_data[0].mod.Ready("data/parts/dustcover/", "AK_6P1_Dustcover");
		this->sight_data.resize(2);
		this->sight_data[0].mod.Ready("data/parts/sight/", "Eotech_XPS3_HoloSight");
		this->sight_data[1].mod.Ready("data/parts/sight/", "AIMPOINT_T1_DotSight");
		this->foregrip_data.resize(2);
		this->foregrip_data[0].mod.Ready("data/parts/foregrip/", "Standard_Vertical_Foregrip");
		this->foregrip_data[1].mod.Ready("data/parts/foregrip/", "Bipod_Vertical_Foregrip");
		this->light_data.resize(1);
		this->light_data[0].mod.Ready("data/parts/light/", "Tactical_Flashlight");
		this->laser_data.resize(1);
		this->laser_data[0].mod.Ready("data/parts/laser/", "NcSTAR_Laser");
		this->gun_data.resize(1);
		this->gun_data[0].mod.Ready("data/gun/", "AKM");
		//MAG�f�[�^
		this->mag_data.resize(1);
		this->mag_data[0].mod.Ready("data/mag/", "AK_55_Magazine");
		//MED�f�[�^
		this->meds_data.resize(1);
		this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
		//���[�h���1
		UIparts->load_window("�A�C�e���I�u�W�F�N�g");
		//GUN�f�[�^2
		std::for_each(this->mazzule_data.begin(), this->mazzule_data.end(), [&](gunparts& g) { g.set_data_mazzule(&g - &this->mazzule_data[0]); });
		std::for_each(this->grip_data.begin(), this->grip_data.end(), [&](gunparts& g) { g.set_data(&g - &this->grip_data[0]); });
		std::for_each(this->uperhandguard_data.begin(), this->uperhandguard_data.end(), [&](gunparts& g) { g.set_data(&g - &this->uperhandguard_data[0]); });
		std::for_each(this->underhandguard_data.begin(), this->underhandguard_data.end(), [&](gunparts& g) { g.set_data(&g - &this->underhandguard_data[0]); });
		std::for_each(this->mount_data.begin(), this->mount_data.end(), [&](gunparts& g) { g.set_data(&g - &this->mount_data[0]); });
		std::for_each(this->stock_data.begin(), this->stock_data.end(), [&](gunparts& g) { g.set_data(&g - &this->stock_data[0]); });
		std::for_each(this->dustcover_data.begin(), this->dustcover_data.end(), [&](gunparts& g) { g.set_data(&g - &this->dustcover_data[0]); });
		std::for_each(this->light_data.begin(), this->light_data.end(), [&](gunparts& g) { g.set_data(&g - &this->light_data[0]); });
		std::for_each(this->laser_data.begin(), this->laser_data.end(), [&](gunparts& g) { g.set_data(&g - &this->laser_data[0]); });
		std::for_each(this->foregrip_data.begin(), this->foregrip_data.end(), [&](gunparts& g) { g.set_data(&g - &this->foregrip_data[0]); });
		std::for_each(this->sight_data.begin(), this->sight_data.end(), [&](gunparts& g) { g.set_data_sight(&g - &this->sight_data[0]); });
		std::for_each(this->mag_data.begin(), this->mag_data.end(), [&](gunparts& g) { g.set_data_mag(&g - &this->mag_data[0]); });
		std::for_each(this->gun_data.begin(), this->gun_data.end(), [&](gunparts& g) { g.set_data_base(&g - &this->gun_data[0]); });
		//MED�f�[�^2
		std::for_each(this->meds_data.begin(), this->meds_data.end(), [&](Meds& g) { g.set_data(&g - &this->meds_data[0]); });
		//�e��
		this->hit_obj.resize(24);
		this->hit_buf = 0;
		std::for_each(this->hit_obj.begin(), this->hit_obj.end(), [&](Hit& h) { h.set(this->hit_pic); });
		//���[�h���2
		UIparts->load_window("�A�C�e���f�[�^");
		//�J��Ԃ�
		do {
			//�}�b�v�ǂݍ���
			mapparts->Ready_map("data/map_new2");
			UIparts->load_window("�}�b�v");
			mapparts->Set_map("data/maps/set.txt", this->item, this->gun_data, this->mag_data, this->meds_data, "data/grassput.bmp");
			//�L�����ݒ�
			chara.resize(mapparts->get_spawn_point().size());
			auto& mine = chara[0];
			for (auto& c : chara) {
				c.set(this->gun_data, 0, this->body_obj, this->body_col);
				//�}�K�W��
				c.add_parts(this->mag_data[0], EnumGunParts::PARTS_MAGAZINE);
			}
			//�J�X�^��
			{
				switchs up;
				switchs down;
				switchs left;
				switchs right;
				switchs shot;
				switchs start;
				gunparts* parts_p = nullptr;
				size_t parts_cat = SIZE_MAX;
				int sel_p = 0;
				int chang = 0;
				int sel_max = 0;
				int chang_max = 0;
				VECTOR_ref viewparts;
				VECTOR_ref camparts;
				float xrad_t = 30.f;
				float yrad_t = -135.f;
				float range_t = 0.25f;
				float range_tgt = 0.25f;
				switchs Rot;
				int sight1_i = 0;
				int sight2_i = 0;
				bool start_b = true;
				float rate = 0.1f;
				{
					SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
					SetMousePoint(deskx / 2, desky / 2);											//
					camera_main.set_cam_info(deg2rad(Drawparts->use_vr ? 90 : Optionparts->fov_pc), 0.1f, 200.f);//1P
					//���C�e�B���O
					Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {});
					SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
					//�K�v�p�[�c����
					{
						//grip
						{
							parts_p = &this->grip_data[0];
							parts_cat = EnumGunParts::PARTS_GRIP;
							mine.delete_parts(parts_cat);
							mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::GRIP_BASE);
						}
						//uperhandguard
						{
							parts_p = &this->uperhandguard_data[0];
							parts_cat = EnumGunParts::PARTS_UPER_HGUARD;
							mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::UPER_HANDGUARD);
						}
						//underhandguard
						{
							parts_p = &this->underhandguard_data[0];
							parts_cat = EnumGunParts::PARTS_UNDER_HGUARD;
							mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::UNDER_HANDGUARD);
						}
					}
				}
				while (ProcessMessage() == 0) {
					const auto fps_ = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						bool changef = false;
						//���Z
						{
							mine.mat_gun = MGetIdent();
							mine.pos_gun = VGet(0, 0, 0);
							mine.set_gun();
							//
							mine.mat_mag = mine.mat_gun;
							mine.pos_mag = mine.base.obj.frame(mine.base.thisparts->frame_s.magazine2_f.first);
							mine.magazine.obj.SetMatrix(mine.mat_mag* MATRIX_ref::Mtrans(mine.pos_mag));


							mine.key_.wkey = (CheckHitKey(KEY_INPUT_W) != 0);
							mine.key_.skey = (CheckHitKey(KEY_INPUT_S) != 0);
							mine.key_.akey = (CheckHitKey(KEY_INPUT_A) != 0);
							mine.key_.dkey = (CheckHitKey(KEY_INPUT_D) != 0);
							mine.key_.shoot = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
							//up.get_in(mine.key_.wkey);
							down.get_in(mine.key_.skey);
							left.get_in(mine.key_.akey);
							right.get_in(mine.key_.dkey);
							shot.get_in(mine.key_.shoot);
							start.get_in(CheckHitKey(KEY_INPUT_SPACE) != 0);
							Rot.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
							if (start_b) {
								changef = true;
								for (auto& s : mine.base.thisparts->select) {
									if (s == 1) {
										mine.gun_stat[mine.base.thisparts->id].select = uint8_t(&s - &mine.base.thisparts->select[0]);
										break;
									}
								}
								start_b = false;
							}

							if (up.push()) {
								sel_p--;
								changef = true;
							}
							if (down.push()) {
								sel_p++;
								chang = 0;
								changef = true;
							}
							if (sel_p < 0) {
								sel_p = sel_max - 1;
							}
							if (sel_p > sel_max - 1) {
								if (down.push()) {
									if (false) {
										start.second = 1;
									}
									else {
										for (int i = 0; i < mine.sight_.size() + 1; i++) {
											mine.delete_parts(EnumGunParts::PARTS_SIGHT, 0);
										}
									}
								}
								sel_p = 0;
							}

							if (left.push()) {
								chang--;
								changef = true;
							}
							if (right.push()) {
								chang++;
								changef = true;
							}
							if (chang < 0) {
								chang = chang_max - 1;
							}
							if (chang > chang_max - 1) {
								chang = 0;
							}

							if (changef) {
								sel_max = 0;
								//grip
								if (sel_p == sel_max) {
									chang_max = (int)this->grip_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									parts_p = &this->grip_data[chang];
									parts_cat = EnumGunParts::PARTS_GRIP;
									mine.delete_parts(parts_cat);
									mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::GRIP_BASE);
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::GRIP_BASE][0].first);
								}
								sel_max++;
								//uperhandguard
								if (sel_p == sel_max) {
									chang_max = (int)this->uperhandguard_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									parts_p = &this->uperhandguard_data[chang];
									parts_cat = EnumGunParts::PARTS_UPER_HGUARD;
									mine.delete_parts(parts_cat);
									mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::UPER_HANDGUARD);
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::UPER_HANDGUARD][0].first);
								}
								sel_max++;
								//underhandguard
								if (sel_p == sel_max) {
									chang_max = (int)this->underhandguard_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									parts_p = &this->underhandguard_data[chang];
									parts_cat = EnumGunParts::PARTS_UNDER_HGUARD;
									mine.delete_parts(parts_cat);
									mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::UNDER_HANDGUARD);
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::UNDER_HANDGUARD][0].first);
								}
								sel_max++;
								{
									if (mine.underhandguard.rail_f[EnumAttachPoint::UNDER][0].first > 0) {
										if (sel_p == sel_max) {
											chang_max = (int)this->foregrip_data.size() + 1;
											chang = std::clamp(chang, 0, chang_max - 1);
											if (chang == 0) {
												parts_p = nullptr;
												parts_cat = EnumGunParts::PARTS_FOREGRIP;
												mine.delete_parts(parts_cat);
											}
											else {
												parts_p = &this->foregrip_data[chang - 1];
												parts_cat = EnumGunParts::PARTS_FOREGRIP;
												mine.delete_parts(parts_cat);
												mine.add_parts(*parts_p, parts_cat, &mine.underhandguard, EnumAttachPoint::UNDER);
											}
											viewparts = mine.underhandguard.obj.frame(mine.underhandguard.rail_f[EnumAttachPoint::UNDER][0].first);
										}
										sel_max++;
									}
									if (mine.underhandguard.rail_f[EnumAttachPoint::LEFTSIDE][0].first > 0) {
										if (sel_p == sel_max) {
											chang_max = (int)(this->light_data.size() + 1 + this->laser_data.size());
											chang = std::clamp(chang, 0, chang_max - 1);
											if (chang == 0) {
												parts_p = nullptr;
												parts_cat = EnumGunParts::PARTS_LIGHT;
												mine.delete_parts(EnumGunParts::PARTS_LIGHT);
												mine.delete_parts(EnumGunParts::PARTS_LASER);
											}
											else {
												if (chang < this->light_data.size() + 1) {
													parts_p = &this->light_data[chang - 1];
													parts_cat = EnumGunParts::PARTS_LIGHT;
												}
												else {
													parts_p = &this->laser_data[chang - 1 - this->light_data.size()];
													parts_cat = EnumGunParts::PARTS_LASER;
												}
												mine.delete_parts(EnumGunParts::PARTS_LIGHT);
												mine.delete_parts(EnumGunParts::PARTS_LASER);
												mine.add_parts(*parts_p, parts_cat, &mine.underhandguard, EnumAttachPoint::LEFTSIDE);
											}
											viewparts = mine.underhandguard.obj.frame(mine.underhandguard.rail_f[EnumAttachPoint::LEFTSIDE][0].first);
										}
										sel_max++;
									}
								}
								//sidemount
								if (sel_p == sel_max) {
									chang_max = (int)2; //chang_max = (int)this->mount_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) {
										parts_p = nullptr;
										parts_cat = EnumGunParts::PARTS_MOUNT;
										mine.delete_parts(parts_cat, 0);
									}
									else if (chang == 1) {
										parts_p = &this->mount_data[0];
										parts_cat = EnumGunParts::PARTS_MOUNT;
										mine.delete_parts(parts_cat, 0);
										mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::SIDEMOUNT_BASE);
									}
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::SIDEMOUNT_BASE][0].first);
								}
								sel_max++;
								if (mine.mount_.size() > 0) {
									if (mine.mount_[0].rail_f[EnumAttachPoint::SIDEMOUNT][0].first > 0) {
										if (sel_p == sel_max) {
											chang_max = (int)1;
											chang = 1;
											//chang_max = (int)this->mount_data.size();
											//chang = std::clamp(chang, 0, chang_max - 1);
											parts_p = &this->mount_data[chang];
											parts_cat = EnumGunParts::PARTS_MOUNT;
											if (mine.mount_.size() == 2) {
												mine.delete_parts(parts_cat, 1);
											}
											mine.add_parts(*parts_p, parts_cat, &mine.mount_[0], EnumAttachPoint::SIDEMOUNT);
											viewparts = mine.mount_[0].obj.frame(mine.mount_[0].rail_f[EnumAttachPoint::SIDEMOUNT][0].first);
										}
										sel_max++;
									}
								}
								//�}�Y��
								if (sel_p == sel_max) {
									chang_max = (int)this->mazzule_data.size() + 1;
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) {
										parts_p = nullptr;
										parts_cat = EnumGunParts::PARTS_MAZZULE;
										mine.delete_parts(parts_cat);
									}
									else {
										parts_p = &this->mazzule_data[chang - 1];
										parts_cat = EnumGunParts::PARTS_MAZZULE;
										mine.delete_parts(parts_cat);
										mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::MAZZULE_);
									}
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::MAZZULE_][0].first);
								}
								sel_max++;
								//�_�X�g�J�o�[
								if (sel_p == sel_max) {
									chang_max = (int)this->dustcover_data.size() + 1;
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) {
										parts_p = nullptr;
										parts_cat = EnumGunParts::PARTS_DUSTCOVER;
										mine.delete_parts(parts_cat);
									}
									else {
										parts_p = &this->dustcover_data[chang - 1];
										parts_cat = EnumGunParts::PARTS_DUSTCOVER;
										mine.delete_parts(parts_cat);
										mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::DUSTCOVER_BASE);
									}
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::DUSTCOVER_BASE][0].first);
								}
								sel_max++;
								//�X�g�b�N
								if (sel_p == sel_max) {
									chang_max = (int)this->stock_data.size() + 1;
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) {
										parts_p = nullptr;
										parts_cat = EnumGunParts::PARTS_STOCK;
										mine.delete_parts(parts_cat);
									}
									else {
										parts_p = &this->stock_data[chang - 1];
										parts_cat = EnumGunParts::PARTS_STOCK;
										mine.delete_parts(parts_cat);
										mine.add_parts(*parts_p, parts_cat, &mine.base, EnumAttachPoint::STOCK_BASE);
									}
									viewparts = mine.base.obj.frame(mine.base.rail_f[EnumAttachPoint::STOCK_BASE][0].first);
								}
								sel_max++;
								//
								{
									auto& parts = mine.uperhandguard;
									if (parts.rail_f[EnumAttachPoint::UPER][0].first > 0) {
										if (sel_p == sel_max) {
											chang_max = (int)this->sight_data.size();
											chang = std::clamp(chang, 0, chang_max - 1);
											parts_p = &this->sight_data[chang];
											parts_cat = EnumGunParts::PARTS_SIGHT;

											if (mine.sight_.size() == sight1_i && sight1_i > 0) {
												mine.delete_parts(parts_cat, sight1_i - 1);
											}

											mine.add_parts(*parts_p, parts_cat, &parts, EnumAttachPoint::UPER);
											viewparts = parts.obj.frame(parts.rail_f[EnumAttachPoint::UPER][0].first);

											sight1_i = (int)mine.sight_.size();
										}
										sel_max++;
									}
								}
								//
								if (mine.mount_.size() > 1) {
									auto& parts = mine.mount_[1];
									if (parts.rail_f[EnumAttachPoint::UPER][0].first > 0) {
										if (sel_p == sel_max) {
											chang_max = (int)this->sight_data.size();
											chang = std::clamp(chang, 0, chang_max - 1);
											parts_p = &this->sight_data[chang];
											parts_cat = EnumGunParts::PARTS_SIGHT;

											if (mine.sight_.size() == sight2_i && sight2_i > 0) {
												mine.delete_parts(parts_cat, sight2_i - 1);
											}

											mine.add_parts(*parts_p, parts_cat, &parts, EnumAttachPoint::UPER);
											viewparts = parts.obj.frame(parts.rail_f[EnumAttachPoint::UPER][0].first);

											sight2_i = (int)mine.sight_.size();
										}
										sel_max++;
									}
								}
								//
								{
									xrad_t = viewparts.y()*1000.f;
									yrad_t = ((viewparts.x() > 0.f) ? 90.f - viewparts.z()*100.f : -90.f + viewparts.z()*100.f);
								}
								//
							}
							//
							if (shot.push()) {
								if (!mine.gunf) {
									//���
									mine.calc_cart(rate);
									//�G�t�F�N�g
									mine.calc_shot_effect();
								}
								//
								mine.gunf = true;
							}

							easing_set(&mine.gunanime_sel[1]->per, float(mine.gun_stat[mine.base.thisparts->id].select / std::max<size_t>(mine.base.thisparts->select.size() - 1, 1)), 0.5f);
							easing_set(&mine.gunanime_sel[0]->per, 1.f - mine.gunanime_sel[1]->per, 0.5f);

							mine.gunanime_shot->per = 1.f;
							if (mine.gunf) {
								mine.gunanime_shot->update(true, rate*2.f*mine.gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
								if (mine.gunanime_shot->time == 0.f) {
									mine.gunf = false;
								}
							}
							//
							mine.base.obj.work_anime();

							//��䰂̏���
							for (auto& a : mine.cart) {
								a.get(mapparts, mine);
							}
							mine.update_effect(Drawparts);
						}
						//campos,camvec,camup�̎w��
						{}
						{
							int x_m, y_m;
							{
								GetMousePoint(&x_m, &y_m);
								x_m -= deskx / 2;
								y_m -= desky / 2;
								SetMousePoint(deskx / 2, desky / 2);
								SetMouseDispFlag(FALSE);
							}
							//
							x_m = std::clamp(x_m, -60, 60);
							y_m = std::clamp(y_m, -60, 60);
							xrad_t = std::clamp(xrad_t + float(y_m)*0.1f, -80.f, 80.f);
							yrad_t += float(x_m)*0.1f;

							yrad_t = rad2deg(atan2f(sin(deg2rad(yrad_t)), cos(deg2rad(yrad_t))));

							auto tmp = viewparts;

							if (std::abs(yrad_t) > 90) {
								if (mine.mazzule.attach) {
									tmp = mine.mazzule.obj.frame(2) - tmp;
								}
								else {
									tmp = mine.base.obj.frame(mine.base.thisparts->frame_s.mazzule_f.first) - tmp;
								}
							}
							else {
								tmp = VECTOR_ref(VGet(0, 0, 0.5f)) - tmp;//todo
							}
							range_tgt = std::hypotf(sin(deg2rad(yrad_t))*0.25f, cos(deg2rad(yrad_t))*(std::abs(tmp.z()) + camera_main.near_ * 2.f));

							if (Rot.on() && !changef) {
								range_t = std::clamp(range_t - float(GetMouseWheelRotVol())*0.1f, range_tgt, 5.f);
							}
							else {
								range_t = range_tgt;
							}

							camera_main.camvec = viewparts;

							easing_set(&camparts, VGet(
								range_t*cos(deg2rad(xrad_t))*sin(deg2rad(yrad_t)),
								range_t*sin(deg2rad(xrad_t)),
								range_t*cos(deg2rad(xrad_t))*cos(deg2rad(yrad_t))),
								0.8f
							);
							camera_main.campos = camera_main.camvec + camparts;
							camera_main.camup = VGet(0, 1.f, 0);
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						//VR��ԂɓK�p
						Drawparts->Move_Player();
						//�`��
						{
							//�e�p��
							Drawparts->Ready_Shadow(camera_main.campos,
								[&] {
								mine.Draw_gun();
								if (mine.magazine.attach) {
									mine.magazine.obj.DrawModel();
								}
							}, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
							//��������
							{
								this->UI_Screen.SetDraw_Screen();
								{
									font18.DrawStringFormat(100, 300, GetColor(255, 0, 0), "total : %d / %d", sel_p + 1, sel_max);
									font18.DrawStringFormat(100, 350, GetColor(255, 0, 0), "chang : %d / %d", chang + 1, chang_max);

									int i = 0;
									for (auto& m : mine.mount_) {
										font18.DrawString(400, 300 + i, m.name, GetColor(255, 0, 0)); i += 20;
									}

									i = 0;
									for (auto& m : mine.sight_) {
										font18.DrawString(700, 300 + i, m.name, GetColor(255, 0, 0)); i += 20;
									}

									if (parts_p != nullptr) {
										font18.DrawString(100, 400, parts_p->mod.name, GetColor(255, 0, 0));
									}

									if (parts_cat != SIZE_MAX) {
										switch (parts_cat) {
										case EnumGunParts::PARTS_MAZZULE:
										{
											font18.DrawString(100, 375, "MAZZULE", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_GRIP:
										{
											font18.DrawString(100, 375, "GRIP", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_UPER_HGUARD:
										{
											font18.DrawString(100, 375, "UPER HANDGUARD", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_UNDER_HGUARD:
										{
											font18.DrawString(100, 375, "UNDER HANDGUARD", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_DUSTCOVER:
										{
											font18.DrawString(100, 375, "DUSTCOVER", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_STOCK:
										{
											font18.DrawString(100, 375, "STOCK", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_LASER:
										{
											font18.DrawString(100, 375, "LASER", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_LIGHT:
										{
											font18.DrawString(100, 375, "LIGHT", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_FOREGRIP:
										{
											font18.DrawString(100, 375, "FOREGRIP", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_MOUNT:
										{
											font18.DrawString(100, 375, "MOUNT", GetColor(255, 0, 0));
											break;
										}
										case EnumGunParts::PARTS_SIGHT:
										{
											font18.DrawString(100, 375, "SIGHT", GetColor(255, 0, 0));
											break;
										}
										default:
											break;
										}
									}

									font18.DrawString(100, 525, "����   :Parts Change", GetColor(255, 0, 0));
									font18.DrawString(100, 550, "��     :Parts Select", GetColor(255, 0, 0));
									font18.DrawString(100, 575, "SPACE  :Go Battle", GetColor(255, 0, 0));
									font18.DrawString(100, 600, (Rot.on()) ? "RANGE  :FREE" : "RANGE  :FIXED", GetColor(255, 0, 0));
								}
							}
							//VR�Ɉڂ�
							Drawparts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									Hostpassparts->BUF_draw(
										[&]() {
										DrawBox(0, 0, 1920, 1080, GetColor(128, 128, 128), TRUE);
										},
										[&]() {
										Drawparts->Draw_by_Shadow(
											[&] {
											mine.Draw_gun();
											if (mine.magazine.attach) {
												mine.magazine.obj.DrawModel();
											}
										});
										}, tmp_cam);
									//�ŏI�`��
									Hostpassparts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									Hostpassparts->get_main().DrawGraph(0, 0, true);
									//UI
									if (Drawparts->use_vr) {
										SetCameraNearFar(0.01f, 2.f);
										SetUseZBuffer3D(FALSE);												//zbufuse
										SetWriteZBuffer3D(FALSE);											//zbufwrite
										{
											DrawBillboard3D((camera_main.campos + (camera_main.camvec - camera_main.campos).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, this->UI_Screen.get(), TRUE);
										}
										SetUseZBuffer3D(TRUE);												//zbufuse
										SetWriteZBuffer3D(TRUE);											//zbufwrite
									}
									else {
										this->UI_Screen.DrawGraph(0, 0, TRUE);
									}
								}
							}, camera_main);
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
						{
							//FPS���_
							if (Drawparts->use_vr) {
								Drawparts->outScreen[0].DrawRotaGraph(960, 540, 0.5f, 0, false);
							}
							else {
								Drawparts->outScreen[0].DrawGraph(0, 0, false);
							}
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//��ʂ̔��f
					Drawparts->Screen_Flip();
					//�I������
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (start.push()) {
						break;
					}
					//
				}
				{
					Drawparts->Delete_Shadow();
				}
			}
			for (auto& c : chara) {
				//�J�X�^��attach
				if (&c != &mine) {
					//grip
					c.add_parts(this->grip_data[0], EnumGunParts::PARTS_GRIP, &c.base, EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c.add_parts(this->uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, &c.base, EnumAttachPoint::UPER_HANDGUARD);
					{
						c.add_parts(this->sight_data[0], EnumGunParts::PARTS_SIGHT, &c.uperhandguard, EnumAttachPoint::UPER);
					}
					//underhandguard
					c.add_parts(this->underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, &c.base, EnumAttachPoint::UNDER_HANDGUARD);
					{
						c.add_parts(this->foregrip_data[0], EnumGunParts::PARTS_FOREGRIP, &c.underhandguard, EnumAttachPoint::UNDER);
						c.add_parts(this->light_data[0], EnumGunParts::PARTS_LIGHT, &c.underhandguard, EnumAttachPoint::LEFTSIDE);
						c.add_parts(this->laser_data[0], EnumGunParts::PARTS_LASER, &c.underhandguard, EnumAttachPoint::RIGHTSIDE);
					}
					//sidemount
					c.add_parts(this->mount_data[0], EnumGunParts::PARTS_MOUNT, &c.base, EnumAttachPoint::SIDEMOUNT_BASE);
					{
						c.add_parts(this->mount_data[1], EnumGunParts::PARTS_MOUNT, &c.mount_[0], EnumAttachPoint::SIDEMOUNT);
						{
							c.add_parts(this->sight_data[1], EnumGunParts::PARTS_SIGHT, &c.mount_[1], EnumAttachPoint::UPER);
						}
					}
					//optional
					c.add_parts(this->mazzule_data[0], EnumGunParts::PARTS_MAZZULE, &c.base, EnumAttachPoint::MAZZULE_);
					{
						c.add_parts(this->dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, &c.base, EnumAttachPoint::DUSTCOVER_BASE);
						{
							c.add_parts(this->stock_data[0], EnumGunParts::PARTS_STOCK, &c.base, EnumAttachPoint::STOCK_BASE);
						}
					}
					//
				}
				//spawn
				{
					auto i = &c - &chara[0];
					c.spawn(mapparts->get_spawn_point()[i], MATRIX_ref::RotY(atan2f(mapparts->get_spawn_point()[i].x(), mapparts->get_spawn_point()[i].z())));
				}
			}
			if (!this->ending) {
				break;
			}
			//���C�e�B���O
			Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] { mapparts->shadow_draw(); });
			SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//�`�悷����̂��w�肷��(��)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					//map
					mapparts->main_draw();
					//������
					std::for_each(this->hit_obj.begin(), this->hit_obj.end(), [&](Hit& s) { s.Draw(); });
					//�T�C�g
					for (auto& s : mine.sight_) {
						s.Draw_reticle(mapparts);
					}
					//�L����
					std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_chara(); });
					//���[�U�[
					std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_laser(chara, mapparts, light); });
					//�A�C�e��
					std::for_each(item.begin(), item.end(), [&](Items& i) { i.Draw_item(mine); });
					//�e�e
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_ammo(); });
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
					//
				});
			};
			//�J�n
			{
				{
					mapparts->Start_map();															//��
					std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.start(mapparts); });//�v���C���[����ϐ��Q
					SetMousePoint(deskx / 2, desky / 2);											//
					camera_main.set_cam_info(deg2rad(Drawparts->use_vr ? 90 : Optionparts->fov_pc), 0.1f, 200.f);//1P
					TPSparts->set(Optionparts->fov_pc);															//TPS
					ruleparts->set();																//���[��
				}
				//
				while (ProcessMessage() == 0) {
					const auto fps_ = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//���W�擾
						if (Drawparts->tracker_num.size() > 0) {
							auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[0]];
							Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &mine.pos_WAIST, &mine.mat_WAIST);
							if (mine.start_c || (ptr_.turn && ptr_.now) != mine.oldv_1_2) {
								mine.pos_WAIST_rep = VGet(mine.pos_WAIST.x(), 0.f, mine.pos_WAIST.z());
							}
							mine.oldv_1_2 = ptr_.turn && ptr_.now;
							mine.pos_WAIST = mine.pos_WAIST - mine.pos_WAIST_rep;
						}
						for (auto& c : chara) {
							c.set_alive(this->item, mapparts);
						}

						//�v���C���[����
						{
							for (auto& c : chara) {
								if (&c == &mine) {
									//mine
									if (Drawparts->use_vr) {
										c.operation_VR(Drawparts, !ruleparts->getstart() || ruleparts->getend());
									}
									else {
										c.operation(!ruleparts->getstart() || ruleparts->getend(), camera_main.fov / deg2rad(Drawparts->use_vr ? 90 : Optionparts->fov_pc));
									}
								}
								else {
									//cpu
									c.operation_NPC(mapparts, chara, !ruleparts->getstart() || ruleparts->getend());
								}
								//common
								c.operation_2();
							}
						}
						//
						for (auto& c : chara) {
							//�ǂ��̑��̔���
							c.wall_col(mapparts, mine, Drawparts->use_vr);
							//body���Z
							if (Drawparts->use_vr && (&c == &mine)) {
								c.move_vr(mapparts, Drawparts);
							}
							else {
								c.move_other(mapparts, mine);
							}
							c.body.work_anime();
							//col���Z
							c.frame_s.copy_frame(c.body, c.colframe_, &c.col);
							c.col.work_anime();
							c.col.RefreshCollInfo(-1, 0);
							c.col.RefreshCollInfo(-1, 1);
							c.col.RefreshCollInfo(-1, 2);
							//
							if (!(Drawparts->use_vr && (&c == &mine))) {
								//���_�擾
								c.pos_HMD = (c.body.frame(c.frame_s.RIGHTeye_f.first) + (c.body.frame(c.frame_s.LEFTeye_f.first) - c.body.frame(c.frame_s.RIGHTeye_f.first))*0.5f) - c.pos;
								//�e��
								c.calc_gun();
							}
							//�ˌ��֘A
							c.calc_shot(mapparts, Drawparts, chara, hit_obj, hit_buf, meds_data, item);
						}
						//�A�C�e�����Z
						for (auto& g : this->item) {
							g.update(item, mapparts);
						}
						//�A�C�e���E��
						for (auto& c : chara) {
							c.canget_gunitem = false;
							c.canget_magitem = false;
							c.canget_meditem = false;
							for (auto& g : this->item) {
								g.Get_item_2(c, mapparts);
							}
						}
						//��}�K�W�����폜����
						while (true) {
							bool demag_flag = false;
							size_t demag_id = 0;
							for (auto& i : this->item) {
								if (i.ptr_mag != nullptr && i.del_timer >= 5.f) {
									i.Delete_item();
									demag_id = &i - &this->item[0];
									demag_flag = true;
									break;
								}
							}
							if (demag_flag) {
								this->item.erase(this->item.begin() + demag_id);
							}
							else {
								break;
							}
						}
						//�������Z�A�A�j���[�V����
						for (auto& c : chara) {
							if (c.start_c) {
								c.body.PhysicsResetState();
								c.start_c = false;
							}
							else  if (c.start_b) {
								c.body.PhysicsResetState();
								c.start_b = false;
							}
							else {
								c.body.PhysicsCalculation(1000.f / fps_);
							}
						}
						//
						for (auto&s : mine.sight_) {
							s.Set_reticle();
						}
						//campos,camvec,camup�̎w��
						{
							auto& tgt = mine;
							tgt.set_cam(mapparts, camera_main, chara, deg2rad(Drawparts->use_vr ? 90 : Optionparts->fov_pc), Drawparts->use_vr);
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						//VR��ԂɓK�p
						Drawparts->Move_Player();
						//���[���ۑ�
						UIparts->set_rule(ruleparts->getready(), ruleparts->gettimer());
						ruleparts->update();
						//
						TPSparts->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
						//�~�j�}�b�v
						{
							auto* cc = &mine;
							if (TPSparts->TPS.on()) {
								cc = &chara[TPSparts->sel_cam];
							}
							minimapparts->set(chara, *cc, mapparts);
						}
						//1P�`��
						{
							//�e�p��
							Drawparts->Ready_Shadow(camera_main.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
								for (auto& g : this->item) { g.Draw_item(); }
							},
								(mine.HP != 0) ? VGet(2.f, 2.5f, 2.f) : VGet(10.f, 2.5f, 10.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//���C�g
							for (auto& c : this->chara) {
								if (&c == &mine) {
									c.Set_Light();
								}
							}
							//��������
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(mine, Drawparts->use_vr);
								}
							}
							{
								cam_info* camp = &camera_main;
								if (TPSparts->TPS.on()) {
									camp = &TPSparts->camera_TPS;
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
								{
									for (auto& c : chara) {
										c.check_CameraViewClip(mapparts);
									}
								}
							}
							//VR�Ɉڂ�
							Drawparts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									Hostpassparts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//�ŏI�`��
									Hostpassparts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									//main
									Hostpassparts->get_main().DrawGraph(0, 0, true);
									//UI
									if (Drawparts->use_vr) {
										SetCameraNearFar(0.01f, 2.f);
										SetUseZBuffer3D(FALSE);												//zbufuse
										SetWriteZBuffer3D(FALSE);											//zbufwrite
										{
											DrawBillboard3D((camera_main.campos + (camera_main.camvec - camera_main.campos).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, this->UI_Screen.get(), TRUE);
										}
										SetUseZBuffer3D(TRUE);												//zbufuse
										SetWriteZBuffer3D(TRUE);											//zbufwrite
									}
									else {
										this->UI_Screen.DrawGraph(0, 0, TRUE);
									}
									//UI2
									for (auto& c : chara) {
										for (auto& a : c.bullet) {
											a.set_draw();
										}
									}
									UIparts->item_draw(chara, mine, this->item, camera_main.campos, Drawparts->use_vr);
								}
							}, camera_main);
						}
						//TPS�`��
						TPSparts->set_info(mapparts, chara);
						if (TPSparts->TPS.on()) {
							//�e�p��
							Drawparts->Ready_Shadow(TPSparts->camera_TPS.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
								for (auto& g : this->item) { g.Draw_item(); }
							},
								VGet(2.f, 2.5f, 2.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//��������
							{
								Hostpassparts_TPS->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, TPSparts->camera_TPS);	//��ʑ̐[�x�`��
								Hostpassparts_TPS->MAIN_draw();																//�ŏI�`��
							}
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
						{
							if (TPSparts->TPS.on()) {
								//TPS���_
								Hostpassparts_TPS->get_main().DrawGraph(0, 0, true);
							}
							else {
								//FPS���_
								if (Drawparts->use_vr) {
									Drawparts->outScreen[0].DrawRotaGraph(960, 540, 0.5f, 0, false);
								}
								else {
									Drawparts->outScreen[0].DrawGraph(0, 0, false);
								}
							}
							//minimap
							minimapparts->draw(64, 64);
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//��ʂ̔��f
					Drawparts->Screen_Flip();
					//�I������
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
					//
				}
				//���
				{
					std::for_each(chara.begin(), chara.end(), [](Chara& c) {c.Delete_chara(); });
					chara.clear();
					std::for_each(item.begin(), item.end(), [](Items& i) {i.Delete_item(); });
					item.clear();
					mapparts->Delete_map();
					Drawparts->Delete_Shadow();
				}
			}
			//
		} while (ProcessMessage() == 0 && this->ending);

		//���(�O�̂���)
		{
			std::for_each(chara.begin(), chara.end(), [](Chara& c) {c.Delete_chara(); });
			chara.clear();
			std::for_each(item.begin(), item.end(), [](Items& i) {i.Delete_item(); });
			item.clear();
			mapparts->Delete_map();
		}
		//

	}
};