#pragma once
// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			0.4f	// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			1.8f	// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��

class MAPclass : Mainclass {
public:
	class Map {
	private:
		std::string path;
		MV1 map, map_col;	//�n��
		MV1 sky;			//��
		SoundHandle envi;	//
		std::vector<VECTOR_ref> way_point;
		std::vector<VECTOR_ref> lean_point_q;
		std::vector<VECTOR_ref> lean_point_e;
		std::vector<VECTOR_ref> spawn_point;
		int grass_pos = -1;
		GraphHandle minmap;
		int x_size = 0;
		int y_size = 0;
		//���z
		GraphHandle sun_pic;			/*�摜�n���h��*/
		VECTOR_ref sun_pos;
		//�_
		int grasss = 2000;				/*grass�̐�*/
		std::vector<VERTEX3D> grassver; /*grass*/
		std::vector<DWORD> grassind;    /*grass*/
		int VerBuf = -1, IndexBuf = -1;	/*grass*/
		MV1 grass;						/*grass���f��*/
		GraphHandle grass_pic;			/*�摜�n���h��*/
		int IndexNum = -1, VerNum = -1;	/*grass*/
		int vnum = -1, pnum = -1;		/*grass*/
		MV1_REF_POLYGONLIST RefMesh;	/*grass*/
	public:
		std::vector<Items> item;					//�A�C�e��
	public:
		std::vector<VECTOR_ref>& get_waypoint(void) { return way_point; }
		std::vector<VECTOR_ref>& get_leanpoint_q(void) { return lean_point_q; }
		std::vector<VECTOR_ref>& get_leanpoint_e(void) { return lean_point_e; }
		std::vector<VECTOR_ref>& get_spawn_point(void) { return spawn_point; }
		GraphHandle& get_minmap(void) { return minmap; }
		int& get_x_size(void) { return x_size; }
		int& get_y_size(void) { return y_size; }
		Map(int grass_level) {
			switch (grass_level) {
			case 0:
				grasss = 0;
				break;
			case 1:
				grasss = 500;
				break;
			case 2:
				grasss = 1000;
				break;
			case 3:
				grasss = 2000;
				break;
			case 4:
				grasss = 5000;
				break;
			default:
				grasss = 0;
				break;
			}
		}
		~Map(void) { }
		void Ready_map(std::string dir) {
			this->path = dir + "/";
			MV1::Load(this->path + "model.mv1", &map, true);		   //map
			MV1::Load(this->path + "col.mv1", &map_col, true);		   //map�R���W����
			MV1::Load(this->path + "sky/model.mv1", &sky, true);	 //��
			SetUseASyncLoadFlag(TRUE);
			envi = SoundHandle::Load(this->path + "envi.wav");
			minmap = GraphHandle::Load(this->path + "minimap.png");

			this->sun_pic = GraphHandle::Load("data/sun.png");					/*sun*/
			SetUseASyncLoadFlag(FALSE);
			if (grasss != 0) {
				SetUseASyncLoadFlag(TRUE);
				grass_pic = GraphHandle::Load("data/model/grass/grass.png");		 /*grass*/
				SetUseASyncLoadFlag(FALSE);
				MV1::Load("data/model/grass/model.mv1", &grass, true);		/*grass*/
				grass_pos = LoadSoftImage((this->path + "grassput.bmp").c_str());
			}
		}
		void Start(std::vector<GUNPARTs>* get_parts_data, std::vector<Meds>* get_meds_data, const VECTOR_ref& ray) {
			this->sun_pos = ray.Norm() * -1500.f;
			//map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			VECTOR_ref size;
			{
				VECTOR_ref sizetmp = map_col.mesh_maxpos(0) - map_col.mesh_minpos(0);
				if (size.x() < sizetmp.x()) { size.x(sizetmp.x()); }
				if (size.y() < sizetmp.y()) { size.y(sizetmp.y()); }
				if (size.z() < sizetmp.z()) { size.z(sizetmp.z()); }
			}
			map_col.SetupCollInfo(std::max(int(size.x() / 5.f), 1), std::max(int(size.y() / 5.f), 1), std::max(int(size.z() / 5.f), 1), 0, 0);
			{
				MV1SetupReferenceMesh(map_col.get(), 0, FALSE);
				auto p = MV1GetReferenceMesh(map_col.get(), 0, FALSE);
				for (int i = 0; i < p.PolygonNum; i++) {
					if (p.Polygons[i].MaterialIndex == 1) {
						way_point.resize(way_point.size() + 1);
						way_point.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
						auto pt = map_col.CollCheck_Line(way_point.back() + VGet(0, 10.f, 0.f), way_point.back() + VGet(0, -10.f, 0.f), 0, 0);
						if (pt.HitFlag) { way_point.back() = pt.HitPosition; }
					}
					else if (p.Polygons[i].MaterialIndex == 2) {
						lean_point_e.resize(lean_point_e.size() + 1);
						lean_point_e.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
						auto pt = map_col.CollCheck_Line(lean_point_e.back() + VGet(0, 10.f, 0.f), lean_point_e.back() + VGet(0, -10.f, 0.f), 0, 0);
						if (pt.HitFlag) { lean_point_e.back() = pt.HitPosition; }
					}
					else if (p.Polygons[i].MaterialIndex == 3) {
						lean_point_q.resize(lean_point_q.size() + 1);
						lean_point_q.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
						auto pt = map_col.CollCheck_Line(lean_point_q.back() + VGet(0, 10.f, 0.f), lean_point_q.back() + VGet(0, -10.f, 0.f), 0, 0);
						if (pt.HitFlag) { lean_point_q.back() = pt.HitPosition; }
					}
					else if (p.Polygons[i].MaterialIndex == 4) {
						spawn_point.resize(spawn_point.size() + 1);
						spawn_point.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
						auto pt = map_col.CollCheck_Line(spawn_point.back() + VGet(0, 10.f, 0.f), spawn_point.back() + VGet(0, -10.f, 0.f), 0, 0);
						if (pt.HitFlag) { spawn_point.back() = pt.HitPosition; }
					}
				}
			}
			SetFogStartEnd(40.0f - 15.f, 40.f);
			SetFogColor(12, 6, 0);
			item.clear();
			{
				int mdata = FileRead_open((this->path + "Set.txt").c_str(), FALSE);
				//item_data magitem
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") == std::string::npos) {
						size_t p1 = 0;
						float p2 = 0.f, p3 = 0.f, p4 = 0.f;
						for (auto& g : *get_parts_data) {
							if (p.find(g.mod.get_name()) != std::string::npos) {
								p1 = g.id_t;
								break;
							}
						}
						p2 = getparams::_float(mdata);
						p3 = getparams::_float(mdata);
						p4 = getparams::_float(mdata);

						item.resize(item.size() + 1);
						item.back().Set_item_magazine(item.size() - 1, &(*get_parts_data)[p1], VGet(p2, p3, p4), VGet(0, 0, 0), MGetIdent());
					}
					else {
						break;
					}
				}
				//item_data meditem
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") == std::string::npos) {
						size_t p1 = 0;
						float p2 = 0.f, p3 = 0.f, p4 = 0.f;
						for (auto& g : *get_meds_data) {
							if (p.find(g.mod.get_name()) != std::string::npos) {
								p1 = g.id_t;
								break;
							}
						}
						p2 = getparams::_float(mdata);
						p3 = getparams::_float(mdata);
						p4 = getparams::_float(mdata);

						item.resize(item.size() + 1);
						item.back().Set_item_med_(item.size() - 1 ,&(*get_meds_data)[p1], VGet(p2, p3, p4), VGet(0, 0, 0), MGetIdent());
					}
					else {
						break;
					}
				}
				FileRead_close(mdata);
			}
			/*minimap*/
			minmap.GetSize(&x_size, &y_size);
			/*grass*/
			if(grasss!=0) {
				MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̍쐬*/
				RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
				IndexNum = RefMesh.PolygonNum * 3 * grasss; /*�C���f�b�N�X�̐����擾*/
				VerNum = RefMesh.VertexNum * grasss;	/*���_�̐����擾*/
				grassver.resize(VerNum);   /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				grassind.resize(IndexNum); /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/

				vnum = 0;
				pnum = 0;

				int xs = 0, ys = 0;
				GetSoftImageSize(grass_pos, &xs, &ys);

				float x_max = map_col.mesh_maxpos(0).x();
				float z_max = map_col.mesh_maxpos(0).z();
				float x_min = map_col.mesh_minpos(0).x();
				float z_min = map_col.mesh_minpos(0).z();

				for (int i = 0; i < grasss; ++i) {

					float x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
					float z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
					int _r_, _g_, _b_, _a_;
					while (true) {
						GetPixelSoftImage(grass_pos, int((x_t - x_min) / (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
						if (_r_ <= 128) {
							break;
						}
						else {
							x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
							z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
						}
					}
					VECTOR_ref tmpvect2 = VGet(x_t, -5.f, z_t);
					VECTOR_ref tmpvect = VGet(x_t, 5.f, z_t);
					map_col_nearest(tmpvect2, &tmpvect);
					//
					MV1SetMatrix(grass.get(), MMult(MMult(MGetRotY(deg2rad(GetRand(90))), MGetScale(VGet(1.f, float(100 - 20 + GetRand(20 * 2)) / 100.f, 1.f))), MGetTranslate(tmpvect.get())));
					//���
					MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*�Q�Ɨp���b�V���̍X�V*/
					RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
					for (size_t j = 0; j < size_t(RefMesh.VertexNum); ++j) {
						auto& g = grassver[j + vnum];
						g.pos = RefMesh.Vertexs[j].Position;
						g.norm = RefMesh.Vertexs[j].Normal;
						g.dif = RefMesh.Vertexs[j].DiffuseColor;
						g.spc = RefMesh.Vertexs[j].SpecularColor;
						g.u = RefMesh.Vertexs[j].TexCoord[0].u;
						g.v = RefMesh.Vertexs[j].TexCoord[0].v;
						g.su = RefMesh.Vertexs[j].TexCoord[1].u;
						g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
					}
					for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
						for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
							grassind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
					}
					vnum += RefMesh.VertexNum;
					pnum += RefMesh.PolygonNum * 3;
				}
				DeleteSoftImage(grass_pos);

				VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, grassver.data(), VerNum, VerBuf);
				SetIndexBufferData(0, grassind.data(), IndexNum, IndexBuf);
			}
		}
		void Set(void) {
			envi.play(DX_PLAYTYPE_LOOP, TRUE);
		}
		void Dispose(void) {
			for (auto& i : item) {
				i.Detach_item();
			}
			item.clear();
			map.Dispose();		   //map
			map_col.Dispose();		   //map�R���W����
			sky.Dispose();	 //��
			envi.Dispose();
			way_point.clear();
			lean_point_q.clear();
			lean_point_e.clear();
			spawn_point.clear();
			minmap.Dispose();
			this->sun_pic.Dispose();
			if (grasss != 0) {
				grass_pic.Dispose();
				grass.Dispose();
				grassver.clear();
				grassind.clear();
			}
		}
		auto& map_get(void) { return map; }
		auto& map_col_get(void) { return map_col; }
		auto map_col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos) {
			return map_col.CollCheck_Line(StartPos, EndPos, 0, 0);
		}
		void map_col_nearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) {
			while (true) {
				auto p = this->map_col_line(StartPos, *EndPos);
				if (p.HitFlag) {
					if (*EndPos == p.HitPosition) {
						break;
					}
					*EndPos = p.HitPosition;
				}
				else {
					break;
				}
			}
		}
		void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) {
			auto MoveVector = *NowPos - OldPos;
			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			auto HitDim = map_col.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
			std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; i++) {
				auto& h_d = HitDim.Dim[i];
				//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
					&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y < OldPos.y() + 1.6f)
					) {
					kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
				}
			}
			// �ǃ|���S���Ƃ̓����蔻�菈��
			if (kabe_.size() > 0) {
				bool HitFlag = false;
				for (auto& KeyBind : kabe_) {
					if (Hit_Capsule_Tri(*NowPos + VGet(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
						HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
						if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
							// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
							*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
							bool j = false;
							for (auto& b_ : kabe_) {
								if (Hit_Capsule_Tri(*NowPos + VGet(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
									j = true;
									break;// �������Ă����烋�[�v���甲����
								}
							}
							if (!j) {
								HitFlag = false;
								break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
							}
						}
						else {
							break;
						}
					}
				}
				//*
				if (
					HitFlag
					) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
					for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
						bool HitF = false;
						for (auto& KeyBind : kabe_) {
							if (Hit_Capsule_Tri(*NowPos + VGet(0.0f, 0.2f, 0.0f), *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
								*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
								bool j = false;
								for (auto& b_ : kabe_) {
									if (Hit_Capsule_Tri(*NowPos + VGet(0.0f, 0.2f, 0.0f), *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
										j = true;
										break;
									}
								}
								if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
									break;
								}
								HitF = true;
							}
						}
						if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
							break;
						}
					}
				}
				//*/
				kabe_.clear();
			}
			MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
		}
		//��`��
		void sky_Draw(void) {
			{
				SetFogEnable(FALSE);
				SetUseLighting(FALSE);
				{
					sky.DrawModel();
					DrawBillboard3D(this->sun_pos.get(), 0.5f, 0.5f, 200.f, 0.f, this->sun_pic.get(), TRUE);
				}
				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
			}
			return;
		}
		void grass_Draw(void) {
			if (grasss != 0) {
				SetFogStartEnd(0.0f, 500.f);
				SetFogColor(192, 192, 192);

				SetDrawAlphaTest(DX_CMP_GREATER, 128);
				//SetUseLighting(FALSE);
				{
					DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, grass_pic.get(), TRUE);
				}
				//SetUseLighting(TRUE);
				SetDrawAlphaTest(-1, 0);
			}
		}
		void Shadow_Draw(void) {
			for (int i = 0; i < 3; i++) {
				map.DrawMesh(i);
			}
			grass_Draw();
		}
		void item_Draw(void) {
			for(auto& i : item){ i.Draw_item(); }
		}
		void main_Draw(void) {
			map.DrawMesh(0);
			map.DrawMesh(1);
			map.DrawMesh(2);
			SetFogEnable(FALSE);
			map.DrawMesh(3);
			SetFogEnable(TRUE);

			grass_Draw();
			item_Draw();
		}
		//
		template<class Chara>
		void Get_item(VECTOR_ref StartPos, VECTOR_ref EndPos,Chara& chara, std::unique_ptr<Map, std::default_delete<Map>>& MAPPTs) {
			chara.get_canget_magitem() = false;
			chara.get_canget_meditem() = false;
			for (auto& g : this->item) {
				g.Get_item_2(StartPos, EndPos, chara, MAPPTs);
			}
		}
	};
	class MiniMap {
	private:
		int x_size = 0;
		int y_size = 0;
		int x_pos = 0;
		int y_pos = 0;
		GraphHandle UI_player;			//�`��X�N���[��
		GraphHandle UI_minimap;			//�`��X�N���[��
		float xcam = 1.f;
		int MaskHandle = -1;
		bool loadmasks = true;
		//
		std::unique_ptr<Map, std::default_delete<Map>>* MAPPTs;
	private:
		void Set_pos_chara(int* xp, int* yp, const VECTOR_ref& chara_pos, float& radp) {
			auto x_2 = chara_pos.x() / (*MAPPTs)->map_col_get().mesh_maxpos(0).x() *((*MAPPTs)->get_x_size() / 2)*xcam;
			auto y_2 = -chara_pos.z() / (*MAPPTs)->map_col_get().mesh_maxpos(0).z() *((*MAPPTs)->get_y_size() / 2)*xcam;

			*xp = int(x_2*cos(radp) - y_2 * sin(radp));
			*yp = int(y_2*cos(radp) + x_2 * sin(radp));
		}
	public:
		MiniMap(std::unique_ptr<Map, std::default_delete<Map>>* MAPPTs_t) {
			SetUseASyncLoadFlag(FALSE);
			CreateMaskScreen();
			MaskHandle = LoadMask("data/UI/testMask.bmp");
			GetMaskSize(&x_size, &y_size, MaskHandle);
			UI_minimap = GraphHandle::Make(x_size, y_size, true);
			UI_player = GraphHandle::Load("data/UI/player.bmp");
			loadmasks = true;
			MAPPTs = MAPPTs_t;
		}
		template<class Chara>
		void Set(std::vector<Chara>&chara, Chara& mine) {
			UI_minimap.SetDraw_Screen(true);
			{
				DrawBox(0, 0, x_size, y_size, GetColor(0, 128, 0), TRUE);
				int xp = 0, yp = 0;
				float radp = 0.f;
				{
					easing_set(&xcam, 1.f + mine.get_pseed_per() * 0.2f, 0.9f);
					radp = -mine.get_body_yrad();
					int xpos = 0, ypos = 0;
					Set_pos_chara(&xpos, &ypos, mine.get_pos(), radp);
					xp = x_size / 2 - xpos;
					yp = y_size / 2 - ypos;
				}

				(*MAPPTs)->get_minmap().DrawRotaGraph(xp, yp, xcam, radp, true);
				for (auto& c : chara) {
					int xpos = 0, ypos = 0;
					Set_pos_chara(&xpos, &ypos, c.get_pos(), radp);
					int xt = xp + xpos;
					int yt = yp + ypos;

					UI_player.DrawRotaGraph(xt, yt, xcam, radp + c.get_body_yrad(), true);
				}
			}
		}
		void Draw(const int& xpos, const int& ypos) {
			if (x_pos != xpos || y_pos != ypos) {
				loadmasks = true;
			}
			if (loadmasks) {
				FillMaskScreen(0);
				DrawMask(xpos, ypos, MaskHandle, DX_MASKTRANS_BLACK);
				loadmasks = false;
			}
			x_pos = xpos;
			y_pos = ypos;

			SetUseMaskScreenFlag(TRUE);
			UI_minimap.DrawGraph(xpos, ypos, true);
			SetUseMaskScreenFlag(FALSE);
		}
	};
};