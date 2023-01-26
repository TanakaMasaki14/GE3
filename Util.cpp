#include"Util.h"
#include<random>

float Random(float num1, float num2) {

	float min, max;

	min = num2;
	max = num1;

	if (num1 < num2) {
		min = num1;
		max = num2;
	}



	//乱数シード生成器
	std::random_device seedGem;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seedGem());
	//乱数範囲の指定
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}