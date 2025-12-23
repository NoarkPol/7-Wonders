//
// Created by choyichi on 2025/12/16.
//

#ifndef SEVEN_WONDERS_DUEL_DATALOADER_H
#define SEVEN_WONDERS_DUEL_DATALOADER_H

#include "SevenWondersDuel/Global.h"
#include "SevenWondersDuel/Card.h"
#include <string>
#include <vector>

namespace SevenWondersDuel {

	class DataLoader {
	public:
		// 从 JSON 文件加载数据
		static bool loadFromFile(const std::string& filepath,
								 std::vector<Card>& outCards,
								 std::vector<Wonder>& outWonders);
	};

}

#endif // SEVEN_WONDERS_DUEL_DATALOADER_H