/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/nancy/graphics.h"

#include "common/error.h"
#include "common/system.h"

#include "graphics/managed_surface.h"

namespace Nancy {

// TODO the original engine uses a sixth byte for the green; 
// so this should be Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0)
// and transColor should be 0x7C0
// so the colors are gonna be slightly wrong for now
//const Graphics::PixelFormat GraphicsManager::pixelFormat = Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0);
//const uint GraphicsManager::transColor = 0x7C0;
const Graphics::PixelFormat GraphicsManager::pixelFormat = Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);
const uint GraphicsManager::transColor = 0x3E0;

GraphicsManager::GraphicsManager(NancyEngine *engine) :
        _engine(engine) {
    _screen.create(640, 480, pixelFormat);
}

void GraphicsManager::init() {  
    Common::SeekableReadStream *chunk = _engine->getBootChunkStream("VIEW");
    viewportDesc = View(chunk);
    uint32 width = viewportDesc.destRight - viewportDesc.destLeft;
    uint32 height = viewportDesc.destBottom - viewportDesc.destTop;
    _background.create(width, height, pixelFormat);

    // TODO make a TBOX struct
    chunk = _engine->getBootChunkStream("TBOX");
    chunk->seek(0x28);
    width = chunk->readUint32LE();
    height = chunk->readUint32LE();
    chunk->seek(0x20);
    width -= chunk->readUint32LE();
    height -= chunk->readUint32LE();
    _frameTextBox.create(width, height, pixelFormat);
}

GraphicsManager::~GraphicsManager() {
    _background.free();
    _frameTextBox.free();
    _screen.free();

    for (auto st : _ZRender) {
        delete st._value.renderFunction;
    }
}

void GraphicsManager::clearZRenderStruct(Common::String name) {
    _ZRender.erase(name);
}

void GraphicsManager::clearZRenderStructs() {
    _ZRender.clear();
}

ZRenderStruct &GraphicsManager::getZRenderStruct(Common::String name) {
    // Creates a new struct if one didn't exist before
    return _ZRender[name];
}

void GraphicsManager::initZRenderStruct(char const *name,
                                        uint32 z,
                                        bool isActive,
                                        ZRenderStruct::BltType bltType,
                                        Graphics::Surface *surface,
                                        RenderFunction *func,
                                        Common::Rect *sourceRect,
                                        Common::Rect *destRect ) {
    clearZRenderStruct(name);
    ZRenderStruct &st = getZRenderStruct(name);
    st.name = name;
    st.z = z;
    st.isActive = isActive;
    st.isInitialized = true;
    st.bltType = bltType;
    st.sourceSurface = surface;
    if (sourceRect)
        st.sourceRect = *sourceRect;
    else st.sourceRect = Common::Rect();
    if (destRect)
        st.destRect = *destRect;
    else st.destRect = Common::Rect();
    st.renderFunction = func;
}

// TODO nancy1 only, move to subclass whenever we support multiple games
// TODO most of these are wrong and/or incomplete
// The original engine uses dirty rectangles for optimization and marks
// their location with zrender structs whose names start with RES.
// I'm using a more naive implementation where everything is redrawn every frame
// for code simplicity, but that can be changed in the future if needed
void GraphicsManager::initSceneZRenderStructs() {
    Common::Rect *source = new Common::Rect();
    Common::Rect *dest = new Common::Rect();
    Common::SeekableReadStream *chunk = nullptr;
    
    #define READ_RECT(where, x) chunk->seek(x); \
                                where->left = chunk->readUint32LE(); \
                                where->top = chunk->readUint32LE(); \
                                where->right = chunk->readUint32LE(); \
                                where->bottom = chunk->readUint32LE();

    chunk = _engine->getBootChunkStream("MENU");
    READ_RECT(source, 16)
    initZRenderStruct(  "FRAME", 1, true, ZRenderStruct::BltType::kNone, &_primaryFrameSurface,
                        new RenderFunction(this, &GraphicsManager::renderFrame), source, source);
    initZRenderStruct(  "CUR IMAGE CURSOR", 11, false, ZRenderStruct::BltType::kTrans, &_object0Surface);

    chunk = _engine->getBootChunkStream("TBOX");
    READ_RECT(source, 0)
    initZRenderStruct(  "CUR TB BAT SLIDER", 9, false, ZRenderStruct::BltType::kTrans,
                        &_object0Surface, nullptr, source, nullptr);

    chunk = _engine->getBootChunkStream("BSUM");
    READ_RECT(dest, 356)
    initZRenderStruct(  "FRAME TB SURF", 6, false, ZRenderStruct::BltType::kNoTrans,
                        &_frameTextBox, nullptr, nullptr, dest);

    READ_RECT(source, 388)
    READ_RECT(dest, 420)
    initZRenderStruct(  "MENU BUT DN", 5, false, ZRenderStruct::BltType::kTrans,
                        &_object0Surface, nullptr, source, dest);

    READ_RECT(source, 404)
    READ_RECT(dest, 436)
    initZRenderStruct(  "HELP BUT DN", 5, false, ZRenderStruct::BltType::kTrans,
                        &_object0Surface, nullptr, source, dest);

    chunk = _engine->getBootChunkStream("INV");
    READ_RECT(source, 0)
    initZRenderStruct(  "CUR INV SLIDER", 9, false, ZRenderStruct::BltType::kTrans,
                         &_object0Surface, nullptr, source, nullptr);

    initZRenderStruct(  "FRAME INV BOX", 6, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderFrameInvBox));
    
    initZRenderStruct(  "INV BITMAP", 9, false, ZRenderStruct::BltType::kNoTrans);
    initZRenderStruct(  "PRIMARY VIDEO", 8, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderPrimaryVideo));
    initZRenderStruct(  "SEC VIDEO 0", 8, false, ZRenderStruct::BltType::kTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderSecVideo0));
    initZRenderStruct(  "SEC VIDEO 1", 8, false, ZRenderStruct::BltType::kTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderSecVideo1));
    initZRenderStruct(  "SEC MOVIE", 8, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderSecMovie));
    initZRenderStruct(  "ORDERING PUZZLE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderOrderingPuzzle));
    initZRenderStruct(  "ROTATING LOCK PUZZLE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderRotatingLockPuzzle));
    initZRenderStruct(  "LEVER PUZZLE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderLeverPuzzle));
    initZRenderStruct(  "TELEPHONE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderTelephone));
    initZRenderStruct(  "SLIDER PUZZLE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderSliderPuzzle));
    initZRenderStruct(  "PASSWORD PUZZLE", 7, false, ZRenderStruct::BltType::kNoTrans, nullptr,
                        new RenderFunction(this, &GraphicsManager::renderPasswordPuzzle));

    // Moved here from SceneManager::load(), should be ok
    *source = Common::Rect(viewportDesc.srcLeft, viewportDesc.srcTop, viewportDesc.srcRight, viewportDesc.srcBottom);
    *dest = Common::Rect(viewportDesc.destLeft, viewportDesc.destTop, viewportDesc.destRight, viewportDesc.destBottom);
    initZRenderStruct(  "VIEWPORT AVF", 6, true, ZRenderStruct::BltType::kNoTrans,
                        &_background, nullptr, source, dest);
    #undef READ_RECT

    delete source;
    delete dest;
}

void GraphicsManager::renderDisplay() {
    // Construct a list containing every struct and pass it along
    Common::Array<Common::String> array;
    for (auto i : _ZRender) {
        array.push_back(i._key);
    }

    renderDisplay(array);
}

void GraphicsManager::renderDisplay(Common::Array<Common::String> ids) {
    for (uint currentZ = _startingZ; currentZ < 12; ++currentZ) {
        for (uint i = 0; i < ids.size(); ++i) {
            ZRenderStruct &current = getZRenderStruct(ids[i]);
            if (current.isActive && current.isInitialized && current.z == currentZ) {
                if (current.renderFunction && current.renderFunction->isValid()) {
                    current.renderFunction->operator()();
                }
                else {
                    switch (current.bltType) {
                        // making some assumptions here
                        case ZRenderStruct::BltType::kNoTrans: {
                            Common::Point dest(current.destRect.left, current.destRect.top);
                            _screen.blitFrom(*current.sourceSurface, current.sourceRect, dest);
                            break;
                        }
                        case ZRenderStruct::BltType::kTrans: {
                            Common::Point dest(current.destRect.left, current.destRect.top);
                            _screen.transBlitFrom(*current.sourceSurface, current.sourceRect, dest, transColor);
                            break;
                        }
                        default:
                            error("Bad ZRender Blt type!");
                    }
                }

                // Current struct has been rendered, remove from list
                ids.remove_at(i);
                break;
            }
        }
    }
    _screen.update();
}

void GraphicsManager::loadBackgroundVideo(const Common::String &filename) {
    if (_videoDecoder.isVideoLoaded()) {
        _videoDecoder.close();
    }
    _videoDecoder.loadFile(filename + ".avf");
}

const Graphics::Surface *GraphicsManager::getBackgroundFrame(uint16 frameId)  {
    if (!_videoDecoder.isVideoLoaded()) {
        error("No video loaded");
        return nullptr;
    }
    return _videoDecoder.decodeFrame(frameId);
}

uint32 GraphicsManager::getBackgroundFrameCount() {
    return _videoDecoder.getFrameCount();
}

uint32 GraphicsManager::getBackgroundWidth() {
    return _videoDecoder.getWidth();
}

uint32 GraphicsManager::getBackgroundHeight() {
    return _videoDecoder.getHeight();
}

void GraphicsManager::renderFrame() {
    ZRenderStruct &zr = getZRenderStruct("FRAME");
    Common::Point dest(zr.destRect.left, zr.destRect.top);
    _screen.blitFrom(*zr.sourceSurface, zr.sourceRect, dest);

    // not sure why we do this
    _numTimesRenderedFrame += 1;
    if (_numTimesRenderedFrame > 1) {
        _startingZ = 2;
        _numTimesRenderedFrame = 0;
    } else {
        _startingZ = 1;
    }
}

void GraphicsManager::renderFrameInvBox() {
    // TODO
}

void GraphicsManager::renderPrimaryVideo() {
    // TODO
}

void GraphicsManager::renderSecVideo0() {
    // TODO
}

void GraphicsManager::renderSecVideo1() {
    // TODO
}

void GraphicsManager::renderSecMovie() {
    // TODO
}

void GraphicsManager::renderOrderingPuzzle() {
    // TODO
}

void GraphicsManager::renderRotatingLockPuzzle() {
    // TODO
}

void GraphicsManager::renderLeverPuzzle() {
    // TODO
}

void GraphicsManager::renderTelephone() {
    // TODO
}

void GraphicsManager::renderSliderPuzzle() {
    // TODO
}

void GraphicsManager::renderPasswordPuzzle() {
    // TODO
}

} // End of namespace Nancy