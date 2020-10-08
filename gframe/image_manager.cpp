#include "image_manager.h"
#include "game.h"

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	RefreshRandomImageList();

	tCover[0] = NULL;
	tCover[1] = NULL;
	tCover[2] = GetRandomImage(TEXTURE_COVER_S, CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	if(!tCover[2])
		tCover[2] = GetTextureFromFile("textures/cover.jpg", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	tCover[3] = GetRandomImage(TEXTURE_COVER_O, CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	if(!tCover[3])
		tCover[3] = GetTextureFromFile("textures/cover2.jpg", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	if(!tCover[3])
		tCover[3] = tCover[2];
	tUnknown = NULL;
	tUnknownFit = NULL;
	tUnknownThumb = NULL;
	tBigPicture = NULL;
	tLoading = NULL;
	tThumbLoadingThreadRunning = false;
	tAct = GetRandomImage(TEXTURE_ACTIVATE);
	tAttack = GetRandomImage(TEXTURE_ATTACK);
	if(!tAct)
		tAct = driver->getTexture("textures/act.png");
	if(!tAttack)
		tAttack = driver->getTexture("textures/attack.png");
	tChain = driver->getTexture("textures/chain.png");
	tNegated = driver->getTexture("textures/negated.png");
	tNumber = driver->getTexture("textures/number.png");
	tLPBar = driver->getTexture("textures/lp.png");
	tLPFrame = driver->getTexture("textures/lpf.png");
	tMask = driver->getTexture("textures/mask.png");
	tEquip = driver->getTexture("textures/equip.png");
	tTarget = driver->getTexture("textures/target.png");
	tChainTarget = driver->getTexture("textures/chaintarget.png");
	tLim = driver->getTexture("textures/lim.png");
	tOT = driver->getTexture("textures/ot.png");
	tHand[0] = driver->getTexture("textures/f1.jpg");
	tHand[1] = driver->getTexture("textures/f2.jpg");
	tHand[2] = driver->getTexture("textures/f3.jpg");
	tBackGround = NULL;
	tBackGround_menu = NULL;
	tBackGround_deck = NULL;
	tCardType = driver->getTexture("textures/cardtype.png");
	tField[0] = driver->getTexture("textures/field2.png");
	tFieldTransparent[0] = driver->getTexture("textures/field-transparent2.png");
	tField[1] = driver->getTexture("textures/field3.png");
	tFieldTransparent[1] = driver->getTexture("textures/field-transparent3.png");
	tField[2] = driver->getTexture("textures/field4.png");
	tFieldTransparent[2] = driver->getTexture("textures/field-transparent4.png");
	char buff[100];
	for (int i = 0; i < 14; i++) {
		snprintf(buff, 100, "textures/pscale/rscale_%d.png", i);
		tRScale[i] = driver->getTexture(buff);
	}
	for (int i = 0; i < 14; i++) {
		snprintf(buff, 100, "textures/pscale/lscale_%d.png", i);
		tLScale[i] = driver->getTexture(buff);
	}
	tClock = driver->getTexture("textures/clock.png");
	ResizeTexture();
	return true;
}
irr::video::ITexture* ImageManager::GetRandomImage(int image_type) {
	int count = ImageList[image_type].size();
	if(count <= 0)
		return NULL;
	char ImageName[1024];
	wchar_t fname[1024];
	if(saved_image_id[image_type] == -1)
		saved_image_id[image_type] = rand() % count;
	int image_id = saved_image_id[image_type];
	auto name = ImageList[image_type][image_id].c_str();
	myswprintf(fname, L"./textures/%ls", name);
	BufferIO::EncodeUTF8(fname, ImageName);
	return driver->getTexture(ImageName);
}
irr::video::ITexture* ImageManager::GetRandomImage(int image_type, s32 width, s32 height) {
	int count = ImageList[image_type].size();
	if(count <= 0)
		return NULL;
	char ImageName[1024];
	wchar_t fname[1024];
	if(saved_image_id[image_type] == -1)
		saved_image_id[image_type] = rand() % count;
	int image_id = saved_image_id[image_type];
	auto name = ImageList[image_type][image_id].c_str();
	myswprintf(fname, L"./textures/%ls", name);
	BufferIO::EncodeUTF8(fname, ImageName);
	return GetTextureFromFile(ImageName, width, height);
}
void ImageManager::RefreshRandomImageList() {
	RefreshImageDir(L"bg", TEXTURE_DUEL);
	RefreshImageDir(L"bg_duel", TEXTURE_DUEL);
	RefreshImageDir(L"bg_deck", TEXTURE_DECK);
	RefreshImageDir(L"bg_menu", TEXTURE_MENU);
	RefreshImageDir(L"cover", TEXTURE_COVER_S);
	RefreshImageDir(L"cover2", TEXTURE_COVER_O);
	RefreshImageDir(L"attack", TEXTURE_ATTACK);
	RefreshImageDir(L"act", TEXTURE_ACTIVATE);

	for(int i = 0; i < 7; ++ i) {
		saved_image_id[i] = -1;
	}
}
void ImageManager::RefreshImageDir(std::wstring path, int image_type) {
	std::wstring search = L"./textures/" + path;
	FileSystem::TraversalDir(search.c_str(), [this, &path, image_type](const wchar_t* name, bool isdir) {
		if(!isdir && wcsrchr(name, '.') && (!mywcsncasecmp(wcsrchr(name, '.'), L".jpg", 4) || !mywcsncasecmp(wcsrchr(name, '.'), L".png", 4))) {
			std::wstring filename = path + L"/" + name;
			ImageList[image_type].push_back(filename);
		}
	});
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
	for(auto tit = tMap[0].begin(); tit != tMap[0].end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tMap[1].begin(); tit != tMap[1].end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tThumb.begin(); tit != tThumb.end(); ++tit) {
		if(tit->second && tit->second != tLoading)
			driver->removeTexture(tit->second);
	}
	if(tBigPicture != NULL) {
		driver->removeTexture(tBigPicture);
		tBigPicture = NULL;
	}
	tMap[0].clear();
	tMap[1].clear();
	tThumb.clear();
	tThumbLoadingMutex.lock();
	tThumbLoading.clear();
	while(!tThumbLoadingCodes.empty())
		tThumbLoadingCodes.pop();
	tThumbLoadingThreadRunning = false;
	tThumbLoadingMutex.unlock();
	tFields.clear();
}
void ImageManager::RemoveTexture(int code) {
	auto tit = tMap[0].find(code);
	if(tit != tMap[0].end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap[0].erase(tit);
	}
	tit = tMap[1].find(code);
	if(tit != tMap[1].end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap[1].erase(tit);
	}
}
void ImageManager::ResizeTexture() {
	irr::s32 imgWidth = CARD_IMG_WIDTH * mainGame->xScale;
	irr::s32 imgHeight = CARD_IMG_HEIGHT * mainGame->yScale;
	irr::s32 imgWidthThumb = CARD_THUMB_WIDTH * mainGame->xScale;
	irr::s32 imgHeightThumb = CARD_THUMB_HEIGHT * mainGame->yScale;
	float mul = (mainGame->xScale > mainGame->yScale) ? mainGame->yScale : mainGame->xScale;
	irr::s32 imgWidthFit = CARD_IMG_WIDTH * mul;
	irr::s32 imgHeightFit = CARD_IMG_HEIGHT * mul;
	irr::s32 bgWidth = 1024 * mainGame->xScale;
	irr::s32 bgHeight = 640 * mainGame->yScale;
	driver->removeTexture(tCover[0]);
	driver->removeTexture(tCover[1]);
	tCover[0] = GetRandomImage(TEXTURE_COVER_S, imgWidth, imgHeight);
	if(!tCover[0])
		tCover[0] = GetTextureFromFile("textures/cover.jpg", imgWidth, imgHeight);
	tCover[1] = GetRandomImage(TEXTURE_COVER_O, imgWidth, imgHeight);
	if(!tCover[1])
		tCover[1] = GetTextureFromFile("textures/cover2.jpg", imgWidth, imgHeight);
	if(!tCover[1])
		tCover[1] = GetRandomImage(TEXTURE_COVER_S, imgWidth, imgHeight);
	if(!tCover[1])
		tCover[1] = tCover[0];
	driver->removeTexture(tUnknown);
	driver->removeTexture(tUnknownFit);
	driver->removeTexture(tUnknownThumb);
	driver->removeTexture(tLoading);
	tUnknown = GetTextureFromFile("textures/unknown.jpg", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	tUnknownFit = GetTextureFromFile("textures/unknown.jpg", imgWidthFit, imgHeightFit);
	tUnknownThumb = GetTextureFromFile("textures/unknown.jpg", imgWidthThumb, imgHeightThumb);
	tLoading = GetTextureFromFile("textures/cover.jpg", imgWidthThumb, imgHeightThumb);
	driver->removeTexture(tBackGround);
	tBackGround = GetRandomImage(TEXTURE_DUEL, bgWidth, bgHeight);
	if(!tBackGround)
		tBackGround = GetTextureFromFile("textures/bg.jpg", bgWidth, bgHeight);
	if(!tBackGround)
		tBackGround = GetTextureFromFile("textures/bg_duel.jpg", bgWidth, bgHeight);
	driver->removeTexture(tBackGround_menu);
	tBackGround_menu = GetRandomImage(TEXTURE_MENU, bgWidth, bgHeight);
	if(!tBackGround_menu)
		tBackGround_menu = GetTextureFromFile("textures/bg_menu.jpg", bgWidth, bgHeight);
	if(!tBackGround_menu)
		tBackGround_menu = GetRandomImage(TEXTURE_DUEL, bgWidth, bgHeight);
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	driver->removeTexture(tBackGround_deck);
	tBackGround_deck = GetRandomImage(TEXTURE_DECK, bgWidth, bgHeight);
	if(!tBackGround_deck)
		tBackGround_deck = GetTextureFromFile("textures/bg_deck.jpg", bgWidth, bgHeight);
	if(!tBackGround_deck)
		tBackGround_deck = GetRandomImage(TEXTURE_DUEL, bgWidth, bgHeight);
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
void imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest) {
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sw = src->getDimension().Width * 1.0;
	double sh = src->getDimension().Height * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<u32> dim = dest->getDimension();
	for(dy = 0; dy < dim.Height; dy++)
		for(dx = 0; dx < dim.Width; dx++) {

			// Calculate floating-point source rectangle bounds.
			minsx = dx * sw / dim.Width;
			maxsx = minsx + sw / dim.Width;
			minsy = dy * sh / dim.Height;
			maxsy = minsy + sh / dim.Height;

			// Total area, and integral of r, g, b values over that area,
			// initialized to zero, to be summed up in next loops.
			area = 0;
			ra = 0;
			ga = 0;
			ba = 0;
			aa = 0;

			// Loop over the integral pixel positions described by those bounds.
			for(sy = floor(minsy); sy < maxsy; sy++)
				for(sx = floor(minsx); sx < maxsx; sx++) {

					// Calculate width, height, then area of dest pixel
					// that's covered by this source pixel.
					pw = 1;
					if(minsx > sx)
						pw += sx - minsx;
					if(maxsx < (sx + 1))
						pw += maxsx - sx - 1;
					ph = 1;
					if(minsy > sy)
						ph += sy - minsy;
					if(maxsy < (sy + 1))
						ph += maxsy - sy - 1;
					pa = pw * ph;

					// Get source pixel and add it to totals, weighted
					// by covered area and alpha.
					pxl = src->getPixel((u32)sx, (u32)sy);
					area += pa;
					ra += pa * pxl.getRed();
					ga += pa * pxl.getGreen();
					ba += pa * pxl.getBlue();
					aa += pa * pxl.getAlpha();
				}

			// Set the destination image pixel to the average color.
			if(area > 0) {
				pxl.setRed(ra / area + 0.5);
				pxl.setGreen(ga / area + 0.5);
				pxl.setBlue(ba / area + 0.5);
				pxl.setAlpha(aa / area + 0.5);
			} else {
				pxl.setRed(0);
				pxl.setGreen(0);
				pxl.setBlue(0);
				pxl.setAlpha(0);
			}
			dest->setPixel(dx, dy, pxl);
		}
}
irr::video::ITexture* ImageManager::GetTextureFromFile(const char* file, s32 width, s32 height) {
	if(mainGame->gameConf.use_image_scale) {
		irr::video::ITexture* texture;
		irr::video::IImage* srcimg = driver->createImageFromFile(file);
		if(srcimg == NULL)
			return NULL;
		if(srcimg->getDimension() == irr::core::dimension2d<u32>(width, height)) {
			texture = driver->addTexture(file, srcimg);
		} else {
			video::IImage *destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<u32>(width, height));
			imageScaleNNAA(srcimg, destimg);
			texture = driver->addTexture(file, destimg);
			destimg->drop();
		}
		srcimg->drop();
		return texture;
	} else {
		return driver->getTexture(file);
	}
}
irr::video::ITexture* ImageManager::GetTexture(int code, bool fit) {
	if(code == 0)
		return fit ? tUnknownFit : tUnknown;
	int width = CARD_IMG_WIDTH;
	int height = CARD_IMG_HEIGHT;
	if(fit) {
		float mul = mainGame->xScale;
		if(mainGame->xScale > mainGame->yScale)
			mul = mainGame->yScale;
		width = width * mul;
		height = height * mul;
	}
	auto tit = tMap[fit ? 1 : 0].find(code);
	if(tit == tMap[fit ? 1 : 0].end()) {
		char file[256];
		sprintf(file, "expansions/pics/%d.png", code);
		irr::video::ITexture* img = GetTextureFromFile(file, width, height);
		if(img == NULL) {
			sprintf(file, "expansions/pics/%d.jpg", code);
			img = GetTextureFromFile(file, width, height);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/%d.png"), code);
			img = GetTextureFromFile(file, width, height);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/%d.jpg"), code);
			img = GetTextureFromFile(file, width, height);
		}
		if(img == NULL) {
			sprintf(file, "pics/%d.png", code);
			img = GetTextureFromFile(file, width, height);
		}
		if(img == NULL) {
			sprintf(file, "pics/%d.jpg", code);
			img = GetTextureFromFile(file, width, height);
		}
		if(img == NULL && !mainGame->gameConf.use_image_scale) {
			tMap[fit ? 1 : 0][code] = NULL;
			return GetTextureThumb(code);
		}
		tMap[fit ? 1 : 0][code] = img;
		return (img == NULL) ? (fit ? tUnknownFit : tUnknown) : img;
	}
	if(tit->second)
		return tit->second;
	else
		return mainGame->gameConf.use_image_scale ? (fit ? tUnknownFit : tUnknown) : GetTextureThumb(code);
}
irr::video::ITexture* ImageManager::GetBigPicture(int code, float zoom) {
	if(code == 0)
		return tUnknown;
	if(tBigPicture != NULL) {
		driver->removeTexture(tBigPicture);
		tBigPicture = NULL;
	}
	irr::video::ITexture* texture;
	char file[256];
	sprintf(file, "expansions/pics/%d.jpg", code);
	irr::video::IImage* srcimg = driver->createImageFromFile(file);
	if(srcimg == NULL) {
		sprintf(file, "pics/%d.jpg", code);
		srcimg = driver->createImageFromFile(file);
	}
	if(srcimg == NULL) {
		return tUnknown;
	}
	if(zoom == 1) {
		texture = driver->addTexture(file, srcimg);
	} else {
		auto origsize = srcimg->getDimension();
		video::IImage* destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<u32>(origsize.Width * zoom, origsize.Height * zoom));
		imageScaleNNAA(srcimg, destimg);
		texture = driver->addTexture(file, destimg);
		destimg->drop();
	}
	srcimg->drop();
	tBigPicture = texture;
	return texture;
}
int ImageManager::LoadThumbThread() {
	while(true) {
		imageManager.tThumbLoadingMutex.lock();
		int code = imageManager.tThumbLoadingCodes.front();
		imageManager.tThumbLoadingCodes.pop();
		imageManager.tThumbLoadingMutex.unlock();
		char file[256];
		sprintf(file, "expansions/pics/thumbnail/%d.png", code);
		irr::video::IImage* img = imageManager.driver->createImageFromFile(file);
		if(img == NULL) {
			sprintf(file, "expansions/pics/thumbnail/%d.jpg", code);
			img = imageManager.driver->createImageFromFile(file);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/thumbnail/%d.png"), code);
			img = imageManager.driver->createImageFromFile(file);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/thumbnail/%d.jpg"), code);
			img = imageManager.driver->createImageFromFile(file);
		}
		if(img == NULL) {
			sprintf(file, "pics/thumbnail/%d.png", code);
			img = imageManager.driver->createImageFromFile(file);
		}
		if(img == NULL) {
			sprintf(file, "pics/thumbnail/%d.jpg", code);
			img = imageManager.driver->createImageFromFile(file);
		}
		if(img == NULL && mainGame->gameConf.use_image_scale) {
			sprintf(file, "expansions/pics/%d.png", code);
			img = imageManager.driver->createImageFromFile(file);
			if(img == NULL) {
				sprintf(file, "expansions/pics/%d.jpg", code);
				img = imageManager.driver->createImageFromFile(file);
			}
			if(img == NULL) {
				sprintf(file, mainGame->GetLocaleDir("pics/%d.png"), code);
				img = imageManager.driver->createImageFromFile(file);
			}
			if(img == NULL) {
				sprintf(file, mainGame->GetLocaleDir("pics/%d.jpg"), code);
				img = imageManager.driver->createImageFromFile(file);
			}
			if(img == NULL) {
				sprintf(file, "pics/%d.png", code);
				img = imageManager.driver->createImageFromFile(file);
			}
			if(img == NULL) {
				sprintf(file, "pics/%d.jpg", code);
				img = imageManager.driver->createImageFromFile(file);
			}
		}
		if(img != NULL) {
			int width = CARD_THUMB_WIDTH * mainGame->xScale;
			int height = CARD_THUMB_HEIGHT * mainGame->yScale;
			if(img->getDimension() == irr::core::dimension2d<u32>(width, height)) {
				img->grab();
				imageManager.tThumbLoadingMutex.lock();
				if(imageManager.tThumbLoadingThreadRunning)
					imageManager.tThumbLoading[code] = img;
				imageManager.tThumbLoadingMutex.unlock();
			} else {
				irr::video::IImage *destimg = imageManager.driver->createImage(img->getColorFormat(), irr::core::dimension2d<u32>(width, height));
				imageScaleNNAA(img, destimg);
				img->drop();
				destimg->grab();
				imageManager.tThumbLoadingMutex.lock();
				if(imageManager.tThumbLoadingThreadRunning)
					imageManager.tThumbLoading[code] = destimg;
				imageManager.tThumbLoadingMutex.unlock();
			}
		} else {
			imageManager.tThumbLoadingMutex.lock();
			if(imageManager.tThumbLoadingThreadRunning)
				imageManager.tThumbLoading[code] = NULL;
			imageManager.tThumbLoadingMutex.unlock();
		}
		imageManager.tThumbLoadingMutex.lock();
		imageManager.tThumbLoadingThreadRunning = !imageManager.tThumbLoadingCodes.empty();
		if(!imageManager.tThumbLoadingThreadRunning)
			break;
		imageManager.tThumbLoadingMutex.unlock();
	}
	imageManager.tThumbLoadingMutex.unlock();
	return 0;
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknownThumb;
	imageManager.tThumbLoadingMutex.lock();
	auto lit = tThumbLoading.find(code);
	if(lit != tThumbLoading.end()) {
		if(lit->second != NULL) {
			char file[256];
			sprintf(file, "pics/thumbnail/%d.jpg", code);
			irr::video::ITexture* texture = driver->addTexture(file, lit->second); // textures must be added in the main thread due to OpenGL
			lit->second->drop();
			tThumb[code] = texture;
		} else {
			tThumb[code] = NULL;
		}
		tThumbLoading.erase(lit);
	}
	imageManager.tThumbLoadingMutex.unlock();
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		tThumb[code] = tLoading;
		imageManager.tThumbLoadingMutex.lock();
		tThumbLoadingCodes.push(code);
		if(!tThumbLoadingThreadRunning) {
			tThumbLoadingThreadRunning = true;
			std::thread(LoadThumbThread).detach();
		}
		imageManager.tThumbLoadingMutex.unlock();
		return tLoading;
	}
	if(tit->second)
		return tit->second;
	else
		return tUnknownThumb;
	if(code == 0)
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		char file[256];
		sprintf(file, "expansions/pics/field/%d.png", code);
		irr::video::ITexture* img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		if(img == NULL) {
			sprintf(file, "expansions/pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/field/%d.png"), code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/field/%d.jpg"), code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.png", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
			if(img == NULL) {
				tFields[code] = NULL;
				return NULL;
			} else {
				tFields[code] = img;
				return img;
			}
		} else {
			tFields[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return NULL;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		char file[256];
		sprintf(file, "expansions/pics/field/%d.png", code);
		irr::video::ITexture* img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		if(img == NULL) {
			sprintf(file, "expansions/pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/field/%d.png"), code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, mainGame->GetLocaleDir("pics/field/%d.jpg"), code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.png", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
			if(img == NULL) {
				tFields[code] = NULL;
				return NULL;
			} else {
				tFields[code] = img;
				return img;
			}
		} else {
			tFields[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}
}
