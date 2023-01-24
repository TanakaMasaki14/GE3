//#include "Particle.h"
//#include<random>
//
////乱数シード生成器
//std::random_device seed_gen;
////メルセンヌツイスター
//std::mt19937_64 engine(seed_gen());
////vecランダム
//std::uniform_real_distribution<float> vecDist(-10.0f, 10.0f);
//
//void Particle::Initialize(const Vec3& pos, const Vec3& vec, const int& timer, const float& scale, const Vec3& angle, const XMFLOAT4& color)
//{
//	this->vec3 = vec;
//	this->timer = timer;
//	this->scale = scale;
//	this->color = color;
//	particle.worldMat->trans = pos;
//	particle.worldMat->rot = {angle};
//	particle.worldMat->SetWorld();
//}
//
//void Particle::Update()
//{
//	vec3 -= vec3.GetNormalized();
//	timer--;
//	scale -= scale / 20.0f;
//
//	particle.worldMat->trans += vec3 * 0.5f;
//	particle.worldMat->scale = { scale,scale,scale };
//	particle.worldMat->rot += {vecDist(engine) / 100.0f, vecDist(engine) / 100.0f, vecDist(engine) / 100.0f};
//	particle.worldMat->SetWorld();
//
//	if (timer <= 0 || scale <= 0)
//	{
//		isDead = true;
//	}
//}
//
//void Particle::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
//{
//	particle.DrawBox(particle.worldMat, &view, &projection, color, texHundle, 1);
//}
//
//bool Particle::GetIsDead()
//{
//	return isDead;
//}
//
//
//
////------------------------------------------------------------------------------------------------------------
//
//void ParticleManager::Initialize()
//{
//	particles.clear();
//}
//
//void ParticleManager::GenerateRandomParticle(const Vec3& pos, const int& deadTimer, const float& scale, const XMFLOAT4& color, int Num)
//{
//	//Numの数だけ生成
//	for (int i = 0; i < Num; i++)
//	{
//		Vec3 v = { vecDist(engine),vecDist(engine) ,vecDist(engine) };
//		//Vec3 rot = { vecDist(engine),vecDist(engine) ,vecDist(engine) };
//		//v.Normalized();
//
//		//生成、初期化
//		std::unique_ptr<Particle> particle = std::make_unique<Particle>();
//		particle->Initialize(pos, v * 1.0f, deadTimer, scale, {0.0f,0.0f,0.0f}, color);
//		//登録
//		particles.push_back(std::move(particle));
//	}
//}
//
//void ParticleManager::Update()
//{
//	//particle
//	for (std::unique_ptr<Particle>& particle : particles)
//	{
//		particle->Update();
//	}
//
//	//particle消す
//	particles.remove_if([](std::unique_ptr<Particle>& particle)
//		{
//			return (particle->GetIsDead());
//		}
//	);
//}
//
//void ParticleManager::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
//{
//	//particle
//	for (std::unique_ptr<Particle>& particle : particles)
//	{
//		particle->Draw(view, projection, texHundle);
//	}
//}
