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
		MV1 map, map_col,mapcol_tank;	//�n��
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
		//kusa
		int grasss = 2000;				/*grass�̐�*/
		GraphHandle grass_pic;			/*�摜�n���h��*/
		MV1 grass;						/*grass���f��*/

		shaders2D shader;				/*�V�F�[�_�[*/
		shaders2D Depth;				/*�V�F�[�_�[*/

		class grass_t {
		public:
			bool canlook = true;

			std::vector<VERTEX3D> grassver; /*grass*/
			std::vector<DWORD> grassind;    /*grass*/
			int VerBuf = -1, IndexBuf = -1;	/*grass*/
			int IndexNum = -1, VerNum = -1;	/*grass*/
			int vnum = -1, pnum = -1;		/*grass*/
			MV1_REF_POLYGONLIST RefMesh;	/*grass*/

			void put_start(const MV1& grass, int total) {
				MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*�Q�Ɨp���b�V���̍X�V*/
				this->RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
				this->IndexNum = this->RefMesh.PolygonNum * 3 * total; /*�C���f�b�N�X�̐����擾*/
				this->VerNum = this->RefMesh.VertexNum * total;	/*���_�̐����擾*/
				this->grassver.resize(this->VerNum);   /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->grassind.resize(this->IndexNum); /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->vnum = 0;
				this->pnum = 0;
			}

			void set_one(const MV1& grass) {
				MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*�Q�Ɨp���b�V���̍X�V*/
				this->RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
				for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
					auto& g = this->grassver[j + this->vnum];
					g.pos = this->RefMesh.Vertexs[j].Position;
					g.norm = this->RefMesh.Vertexs[j].Normal;
					g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
					g.spc = this->RefMesh.Vertexs[j].SpecularColor;
					g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
					g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
					g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
					g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
				}
				for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
					for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
						this->grassind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
				}
				this->vnum += this->RefMesh.VertexNum;
				this->pnum += this->RefMesh.PolygonNum * 3;
			}
			void put() {
				canlook = true;
				this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, this->grassver.data(), this->VerNum, this->VerBuf);
				SetIndexBufferData(0, this->grassind.data(), this->IndexNum, this->IndexBuf);
			}

			/*���E�O���ۂ��𔻒f*/
			void Check_CameraViewClip(int x_, int z_, std::shared_ptr<Map>& MAPPTs, bool use_occlusion) noexcept {
				int pos_xmax = 100 * 1 / 3;
				int pos_xmin = 100 * (x_ + 0) / 3 - 50;
				int pos_zmax = 100 * 1 / 3;
				int pos_zmin = 100 * (z_ + 0) / 3 - 50;
				float x_tmax = (float)((int(MAPPTs->x_max - MAPPTs->x_min) * pos_xmax) + int(MAPPTs->x_max - MAPPTs->x_min) * pos_xmin) / 100.0f;
				float z_tmax = (float)((int(MAPPTs->z_max - MAPPTs->z_min) * pos_zmax) + int(MAPPTs->z_max - MAPPTs->z_min) * pos_zmin) / 100.0f;
				float x_tmin = (float)(int(MAPPTs->x_max - MAPPTs->x_min) * pos_xmin) / 100.0f;
				float z_tmin = (float)(int(MAPPTs->z_max - MAPPTs->z_min) * pos_zmin) / 100.0f;
				VECTOR_ref min = VECTOR_ref::vget(x_tmin, -5.f, z_tmin);
				VECTOR_ref max = VECTOR_ref::vget(x_tmax, 5.f, z_tmax);

				this->canlook = true;

				float range = std::hypotf((MAPPTs->x_max - MAPPTs->x_min) / 3.f, (MAPPTs->z_max - MAPPTs->z_min) / 3.f);

				if ((min - GetCameraPosition()).size() > range && (max - GetCameraPosition()).size() > range) {
					this->canlook = false;
					return;
				}
				if (CheckCameraViewClip_Box(min.get(), max.get())) {
					this->canlook = false;
					return;
				}
				if (use_occlusion) {
					auto ttt = (max - min)*0.5f + min;
					ttt.y(0.f);
					if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 1.8f, 0)).HitFlag &&
						MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.f, 0)).HitFlag) {
						this->canlook = false;
						return;
					}
				}
			}
		};
	public:

		GraphHandle DepthScreen;		// �[�x���擾����X�N���[��

		float x_max = 0.f;
		float z_max = 0.f;
		float x_min = 0.f;
		float z_min = 0.f;
		std::array<std::array<grass_t, 3>, 3>grass__;
		std::vector<std::shared_ptr<Items>> item;					//�A�C�e��


		std::vector<Mainclass::wallPats> wall;					//�ǂ��Z�b�g

	public:
		std::vector<VECTOR_ref>& get_waypoint(void) noexcept { return way_point; }
		std::vector<VECTOR_ref>& get_leanpoint_q(void) noexcept { return lean_point_q; }
		std::vector<VECTOR_ref>& get_leanpoint_e(void) noexcept { return lean_point_e; }
		std::vector<VECTOR_ref>& get_spawn_point(void) noexcept { return spawn_point; }
		GraphHandle& get_minmap(void) noexcept { return minmap; }
		int& get_x_size(void) noexcept { return x_size; }
		int& get_y_size(void) noexcept { return y_size; }
		Map(int grass_level) noexcept {
			switch (grass_level) {
			case 0:
				grasss = 0;
				break;
			case 1:
				grasss = 500 / 9;
				break;
			case 2:
				grasss = 1000 / 9;
				break;
			case 3:
				grasss = 2000 / 9;
				break;
			case 4:
				grasss = 4000 / 9;
				break;
			default:
				grasss = 0;
				break;
			}
		}
		~Map(void) noexcept { }
		void Ready_map(std::string dir, int dispx,int dispy) noexcept {
			this->path = dir + "/";
			MV1::Load(this->path + "model.mv1", &map, true);		   //map
			MV1::Load(this->path + "col.mv1", &map_col, true);		   //map�R���W����
			MV1::Load(this->path + "col_tank.mv1", &mapcol_tank, true);		   //map�R���W����
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

			// �[�x��`�悷��e�N�X�`���̍쐬( �P�`�����l�����������_�P�U�r�b�g�e�N�X�`�� )
			{
				SetCreateDrawValidGraphChannelNum(1);
				SetDrawValidFloatTypeGraphCreateFlag(TRUE);
				SetCreateGraphChannelBitDepth(16);
				DepthScreen = GraphHandle::Make(dispx, dispy, false);
				SetCreateDrawValidGraphChannelNum(0);
				SetDrawValidFloatTypeGraphCreateFlag(FALSE);
				SetCreateGraphChannelBitDepth(0);
			}
			shader.Init("NormalMesh_PointLightVS.vso", "NormalMesh_PointLightPS.pso");
			Depth.Init("DepthVS.vso", "DepthPS.pso");
		}
		void Start(std::unique_ptr<b2World>& world) noexcept {
			//map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			VECTOR_ref size;
			{
				auto sizetmp = map_col.mesh_maxpos(0) - map_col.mesh_minpos(0);
				if (size.x() < sizetmp.x()) { size.x(sizetmp.x()); }
				if (size.y() < sizetmp.y()) { size.y(sizetmp.y()); }
				if (size.z() < sizetmp.z()) { size.z(sizetmp.z()); }
			}
			map_col.SetupCollInfo(std::max(int(size.x() / 5.f), 1), std::max(int(size.y() / 5.f), 1), std::max(int(size.z() / 5.f), 1), 0, 0);
			{
				MV1SetupReferenceMesh(map_col.get(), 0, FALSE);
				auto p = MV1GetReferenceMesh(map_col.get(), 0, FALSE);
				MV1_COLL_RESULT_POLY pt;
				for (int i = 0; i < p.PolygonNum; ++i) {
					switch (p.Polygons[i].MaterialIndex) {
					case 1:
					{
						way_point.emplace_back((VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f));
						pt = map_col_line(way_point.back() + VECTOR_ref::vget(0, 10.f, 0.f), way_point.back() + VECTOR_ref::vget(0, -10.f, 0.f));
						if (pt.HitFlag) { way_point.back() = pt.HitPosition; }
						break;
					}
					case 2:
					{
						lean_point_e.emplace_back((VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f));
						pt = map_col_line(lean_point_e.back() + VECTOR_ref::vget(0, 10.f, 0.f), lean_point_e.back() + VECTOR_ref::vget(0, -10.f, 0.f));
						if (pt.HitFlag) { lean_point_e.back() = pt.HitPosition; }
						break;
					}
					case 3:
					{
						lean_point_q.emplace_back((VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f));
						pt = map_col_line(lean_point_q.back() + VECTOR_ref::vget(0, 10.f, 0.f), lean_point_q.back() + VECTOR_ref::vget(0, -10.f, 0.f));
						if (pt.HitFlag) { lean_point_q.back() = pt.HitPosition; }
						break;
					}
					case 4:
					{
						spawn_point.emplace_back((VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f));
						pt = map_col_line(spawn_point.back() + VECTOR_ref::vget(0, 10.f, 0.f), spawn_point.back() + VECTOR_ref::vget(0, -10.f, 0.f));
						if (pt.HitFlag) { spawn_point.back() = pt.HitPosition; }
						break;
					}
					default:
						break;
					}
				}
			}
			{
				MV1SetupReferenceMesh(mapcol_tank.get(), 0, FALSE);
				MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(mapcol_tank.get(), 0, FALSE);

				for (int i = 0; i < p.PolygonNum; i++) {
					//��
					{
						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}

						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}

						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}
					}
				}

				for (auto& w : wall) {
					std::array<b2Vec2, 2> vs;
					vs[0].Set(w.pos[0].x(), w.pos[0].z());
					vs[1].Set(w.pos[1].x(), w.pos[1].z());
					b2ChainShape chain;		// This a chain shape with isolated vertices
					chain.CreateChain(&vs[0], 2);
					b2FixtureDef fixtureDef;			       /*���I�{�f�B�t�B�N�X�`�����`���܂�*/
					fixtureDef.shape = &chain;			       /**/
					fixtureDef.density = 1.0f;			       /*�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�܂�*/
					fixtureDef.friction = 0.3f;			       /*�f�t�H���g�̖��C���I�[�o�[���C�h���܂�*/
					b2BodyDef bodyDef;				       /*�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo���܂�*/
					bodyDef.type = b2_staticBody;			       /**/
					bodyDef.position.Set(0, 0);			       /**/
					bodyDef.angle = 0.f;				       /**/
					w.b2.Set(world->CreateBody(&bodyDef), &chain);
				}
			}
			SetFogStartEnd(40.0f - 15.f, 40.f);
			SetFogColor(12, 6, 0);
			item.clear();
			/*minimap*/
			minmap.GetSize(&x_size, &y_size);
			/*grass*/
			if (grasss != 0) {
				int xs = 0, ys = 0;
				GetSoftImageSize(grass_pos, &xs, &ys);
				x_max = map_col.mesh_maxpos(0).x();
				z_max = map_col.mesh_maxpos(0).z();
				x_min = map_col.mesh_minpos(0).x();
				z_min = map_col.mesh_minpos(0).z();

				for (int x_ = 0; x_ < 3; x_++) {
					for (int z_ = 0; z_ < 3; z_++) {
						auto& tgt_g = grass__[x_][z_];
						int pos_xmax = 100 * 1 / 3;
						int pos_xmin = 100 * (x_ + 0) / 3 - 50;
						int pos_zmax = 100 * 1 / 3;
						int pos_zmin = 100 * (z_ + 0) / 3 - 50;

						tgt_g.put_start(grass, grasss);
						for (int i = 0; i < grasss; ++i) {
							float x_t = (float)(GetRand(int((x_max - x_min)) * pos_xmax) + int(x_max - x_min) * pos_xmin) / 100.0f;
							float z_t = (float)(GetRand(int((z_max - z_min)) * pos_zmax) + int(z_max - z_min) * pos_zmin) / 100.0f;
							int _r_, _g_, _b_, _a_;
							while (true) {
								GetPixelSoftImage(grass_pos, int((x_t - x_min) / (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
								if (_r_ <= 128) {
									break;
								}
								else {
									x_t = (float)(GetRand(int((x_max - x_min)) * pos_xmax) + int(x_max - x_min) * pos_xmin) / 100.0f;
									z_t = (float)(GetRand(int((z_max - z_min)) * pos_zmax) + int(z_max - z_min) * pos_zmin) / 100.0f;
								}
							}
							auto tmpvect2 = VECTOR_ref::vget(x_t, -5.f, z_t);
							auto tmpvect = VECTOR_ref::vget(x_t, 5.f, z_t);
							map_col_nearest(tmpvect2, &tmpvect);
							//
							grass.SetMatrix((MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(VECTOR_ref::vget(1.f, float(100 - 20 + GetRand(20 * 2)) / 100.f, 1.f))) *MATRIX_ref::Mtrans(tmpvect));
							//���
							tgt_g.set_one(grass);
						}
						tgt_g.put();
					}
				}
				//
				DeleteSoftImage(grass_pos);
			}
		}
		void Start_Ray(const VECTOR_ref& ray) noexcept {
			this->sun_pos = ray.Norm() * -1500.f;
		}
		void Set(void) noexcept {
			envi.play(DX_PLAYTYPE_LOOP, TRUE);
		}
		void Dispose(void) noexcept {
			for (auto& i : item) { i->Detach_item(); }
			item.clear();
			map.Dispose();		   //map
			map_col.Dispose();		   //map�R���W����
			mapcol_tank.Dispose();
			for (auto& w : wall) { w.b2.Dispose(); }
			wall.clear();
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
				for (int x_ = 0; x_ < 3; x_++) {
					for (int z_ = 0; z_ < 3; z_++) {
						grass__[x_][z_].grassver.clear();
						grass__[x_][z_].grassind.clear();
					}
				}
			}
		}
		auto& map_get(void) const noexcept { return map; }
		auto& map_col_get(void) const noexcept { return map_col; }
		const MV1_COLL_RESULT_POLY map_col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos) const noexcept {
			return map_col.CollCheck_Line(StartPos, EndPos, 0, 0);
		}
		bool map_col_nearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
			bool ans = false;
			while (true) {
				MV1_COLL_RESULT_POLY p = this->map_col_line(StartPos, *EndPos);
				if (p.HitFlag) {
					ans = true;
					if (*EndPos == p.HitPosition) {
						break;
					}
					*EndPos = p.HitPosition;
				}
				else {
					break;
				}
			}
			return ans;
		}
		void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) noexcept {
			auto MoveVector = *NowPos - OldPos;
			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			auto HitDim = map_col.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
			std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; ++i) {
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
					if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
						HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
						if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
							// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
							*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
							bool j = false;
							for (auto& b_ : kabe_) {
								if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
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
					for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
						bool HitF = false;
						for (auto& KeyBind : kabe_) {
							if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
								*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
								bool j = false;
								for (auto& b_ : kabe_) {
									if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
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
		void sky_Draw(void) noexcept {
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
		void grass_Draw(void) noexcept {
			if (grasss != 0) {
				SetFogStartEnd(0.0f, 500.f);
				SetFogColor(192, 192, 192);

				SetDrawAlphaTest(DX_CMP_GREATER, 128);
				//SetUseLighting(FALSE);
				{
					for (int x_ = 0; x_ < 3; x_++) {
						for (int z_ = 0; z_ < 3; z_++) {
							if (grass__[x_][z_].canlook) {
								DrawPolygonIndexed3D_UseVertexBuffer(grass__[x_][z_].VerBuf, grass__[x_][z_].IndexBuf, grass_pic.get(), TRUE);
							}
						}
					}
				}
				//SetUseLighting(TRUE);
				SetDrawAlphaTest(-1, 0);
			}
		}
		void Shadow_Draw_Far(void) noexcept {
			for (int i = 0; i < 3; ++i) {
				map.DrawMesh(i);
			}
			grass_Draw();
		}
		void item_Draw(void) noexcept {
			for (auto& i : item) { i->Draw_item(); }
		}
		void main_Draw(void) noexcept {
			for (int i = 0; i < 3; ++i) {
				map.DrawMesh(i);
			}

			shader.draw_lamda(
				[&]() {
				SetUseTextureToShader(1, DepthScreen.get());
				map.DrawMesh(3);
			}
			);

			grass_Draw();
			item_Draw();
		}
		//�[�x�l�`��
		void DepthDraw(bool ischangescreen) {
			int handle = GetDrawScreen();
			DepthScreen.SetDraw_Screen(GetCameraPosition(), GetCameraTarget(), GetCameraUpVector(), GetCameraFov(), GetCameraNear(), GetCameraFar());
			{
				Depth.draw_lamda(
					[&]() {
					SetUseTextureToShader(0, -1);
					map.DrawMesh(0);
				}
				);
			}
			if (ischangescreen) {
				GraphHandle::SetDraw_Screen(handle, GetCameraPosition(), GetCameraTarget(), GetCameraUpVector(), GetCameraFov(), GetCameraNear(), GetCameraFar());
			}
		}
		//
		template<class Chara>
		void Get_item(VECTOR_ref StartPos, VECTOR_ref EndPos, const std::shared_ptr<Chara>& chara, std::shared_ptr<Map>& MAPPTs) noexcept {
			chara->reset_canget_item();
			for (auto& g : this->item) { g->Get_item_2(StartPos, EndPos, chara, MAPPTs); }
		}
		//
		template<size_t T>
		int get_next_waypoint(std::array<int, T> wayp_pre, VECTOR_ref poss) {
			int now = -1;
			auto tmp = VECTOR_ref::vget(0, 100.f, 0);
			for (auto& w : way_point) {
				auto id = &w - &way_point[0];
				bool tt = true;
				for (auto& ww : wayp_pre) {
					if (id == ww) {
						tt = false;
					}
				}
				if (tt) {
					if (tmp.size() >= (w - poss).size()) {
						auto p = map_col_line(w + VECTOR_ref::vget(0, 0.5f, 0), poss + VECTOR_ref::vget(0, 0.5f, 0));
						if (!p.HitFlag) {
							tmp = (w - poss);
							now = int(id);
						}
					}
				}
			}
			return now;
		}

		void Check_CameraViewClip(std::shared_ptr<Map>& MAPPTs, bool use_occlusion) {
			for (int x_ = 0; x_ < 3; x_++) {
				for (int z_ = 0; z_ < 3; z_++) {
					this->grass__[x_][z_].Check_CameraViewClip(x_, z_, MAPPTs, use_occlusion);
				}
			}
			DepthDraw(false);
		}
	};
	class MiniMap {
	private:
		bool usemasks = false;

		int x_size = 0;
		int y_size = 0;
		GraphHandle UI_player;			//�`��X�N���[��
		GraphHandle UI_minimap;			//�`��X�N���[��
		float xcam = 1.f;
		int MaskHandle = -1;
		bool loadmasks = true;
		//
		std::shared_ptr<Map> MAPPTs;
	private:
		void Set_pos_chara(int* xp, int* yp, const VECTOR_ref& chara_pos, float& radp) noexcept {
			auto x_2 = chara_pos.x() / MAPPTs->map_col_get().mesh_maxpos(0).x() *(MAPPTs->get_x_size() / 2)*xcam;
			auto y_2 = -chara_pos.z() / MAPPTs->map_col_get().mesh_maxpos(0).z() *(MAPPTs->get_y_size() / 2)*xcam;

			*xp = int(x_2*cos(radp) - y_2 * sin(radp));
			*yp = int(y_2*cos(radp) + x_2 * sin(radp));
		}
	public:
		MiniMap(std::shared_ptr<Map> MAPPTs_t) noexcept {
			SetUseASyncLoadFlag(FALSE);
			if (usemasks) {
				CreateMaskScreen();
				MaskHandle = LoadMask("data/UI/testMask.bmp");
				GetMaskSize(&x_size, &y_size, MaskHandle);
				loadmasks = true;
			}
			else {
				x_size = 300;
				y_size = 300;
				loadmasks = false;
			}
			UI_minimap = GraphHandle::Make(x_size, y_size, true);
			UI_player = GraphHandle::Load("data/UI/player.bmp");
			MAPPTs = MAPPTs_t;
		}
		template<class Chara>
		void UI_Draw(std::vector<std::shared_ptr<Chara>>&chara, const std::shared_ptr<Chara>& mine) noexcept {
			UI_minimap.SetDraw_Screen(true);
			{
				DrawBox(0, 0, x_size, y_size, GetColor(0, 128, 0), TRUE);
				int xp = 0, yp = 0;
				float radp = 0.f;
				{
					easing_set(&xcam, 1.f + mine->get_pseed_per() * 0.2f, 0.9f);
					radp = -mine->get_body_yrad();
					int xpos = 0, ypos = 0;
					Set_pos_chara(&xpos, &ypos, mine->get_pos(), radp);
					xp = x_size / 2 - xpos;
					yp = y_size / 2 - ypos;
				}

				MAPPTs->get_minmap().DrawRotaGraph(xp, yp, xcam, radp, true);
				for (auto& c : chara) {
					int xpos = 0, ypos = 0;
					Set_pos_chara(&xpos, &ypos, c->get_pos(), radp);
					int xt = xp + xpos;
					int yt = yp + ypos;

					UI_player.DrawRotaGraph(xt, yt, xcam, radp + c->get_body_yrad(), true);
				}
			}
		}
		void Draw(void) noexcept {
			int xpos = y_r(64);
			int ypos = y_r(64);
			int xpos2 = xpos + y_r(x_size);
			int ypos2 = ypos + y_r(y_size);

			if (usemasks) {
				if (loadmasks) {
					FillMaskScreen(0);
					DrawMask(xpos, ypos, MaskHandle, DX_MASKTRANS_BLACK);
					loadmasks = false;
				}
				SetUseMaskScreenFlag(TRUE);
			}
			UI_minimap.DrawExtendGraph(xpos, ypos, xpos2, ypos2, true);
			if (usemasks) {
				SetUseMaskScreenFlag(FALSE);
			}
		}
	};
};