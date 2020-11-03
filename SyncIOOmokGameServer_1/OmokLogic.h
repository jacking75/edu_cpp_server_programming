#pragma once

#include <vector>
#include <string>
#include <memory>
#include "ErrorCode.h"

namespace ChatServerLib
{
	class OmokLogic {
	public:
		OmokLogic();
		virtual ~OmokLogic();

		bool ConfirmOmok(OmokPanPoint omokPanPoints[19][19], int x, int y, OmokPanPoint::PointType pointType);
		bool ConfirmThreeThree(OmokPanPoint omokPanPoints[19][19], int x, int y, OmokPanPoint::PointType pointType);
		int ConfirmHorizontal(OmokPanPoint omokPanPoints[19][19], int x, int y, OmokPanPoint::PointType pointType);
		int ConfirmVertical(OmokPanPoint omokPanPoints[19][19], int x, int y, OmokPanPoint::PointType pointType);
	};
}

