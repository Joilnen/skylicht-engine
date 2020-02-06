/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "IRenderPipeline.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	class CBaseRP : public IRenderPipeline
	{
	protected:
		IRenderPipeline *m_next;

		CMeshBuffer<video::S3DVertex2TCoords> *m_drawBuffer;
		IVertexBuffer* m_verticesImage;
		IIndexBuffer* m_indicesImage;

		float m_viewport2DW;
		float m_viewport2DH;

		SMaterial m_unbindMaterial;
		bool m_updateEntity;

	public:
		CBaseRP();

		virtual ~CBaseRP();

		virtual bool canRenderMaterial(CMaterial *m);

		virtual void render(ITexture *target, CCamera *camera, CEntityManager *entityManager) = 0;

		virtual void setCamera(CCamera *camera);

		virtual void setNextPipeLine(IRenderPipeline *next);

		virtual void onNext(ITexture *target, CCamera *camera, CEntityManager* entity);

		virtual void drawMeshBuffer(CMesh *mesh, int bufferID);

	public:

		void beginRender2D(float w, float h);

		void renderBufferToTarget(float sx, float sy, float sw, float sh, SMaterial& material, bool flipY = true, bool flipX = false);

		void unbindRTT();

		void enableUpdateEntity(bool b)
		{
			m_updateEntity = b;
		}
	};
}