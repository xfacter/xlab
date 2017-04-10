void xDrawStar(float radius, u16 sides, u16 add)
{
    if (sides <= 2 || add <= 0) return;
    float angle = 0.0f;
    int i;
    if (add != 1 && sides % add == 0)
    {
        float angle_between = 2*X_PI/(sides/add)/add;
        sceGumPushMatrix();
        for (i = 0; i < add; i++)
        {
            xDrawStar(radius, sides/add, 1);
            sceGumRotateZ(angle_between);
        }
        sceGumPopMatrix();
        return;
    }
    float angle_increment = add * (2*X_PI/sides);
    VertexF* vertices = (VertexF*)sceGuGetMemory((sides+1)*sizeof(VertexF));
    for (i = 0; i < sides; i++)
    {
        vertices[i].x = radius*x_cosf(angle);
        vertices[i].y = -radius*x_sinf(angle);
        vertices[i].z = 0.0f;
        angle += angle_increment;
    }
    vertices[sides] = vertices[0];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_LINE_STRIP, VertexF_vtype|GU_TRANSFORM_3D, sides+1, 0, vertices);
    xGuLoadStates();
}

void xDrawStarFill(float radius, u16 sides, u16 add)
{
    if (sides <= 2 || add <= 0) return;
    float angle = 0.0f;
    int i;
    if (add != 1 && sides % add == 0)
    {
        float angle_between = 2*X_PI/sides;
        sceGumPushMatrix();
        for (i = 0; i < add; i++)
        {
            xDrawStarFill(radius, sides/add, 1);
            sceGumRotateZ(angle_between);
        }
        sceGumPopMatrix();
        return;
    }
    float angle_increment = add * (2*X_PI/sides);
    VertexF* vertices = (VertexF*)sceGuGetMemory((sides+2)*sizeof(VertexF));
    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 0.0f;
    for (i = 1; i < sides+1; i++)
    {
        vertices[i].x = radius*x_cosf(angle);
        vertices[i].y = -radius*x_sinf(angle);
        vertices[i].z = 0.0f;
        angle += angle_increment;
    }
    vertices[sides+1] = vertices[1];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_TRIANGLE_FAN, VertexF_vtype|GU_TRANSFORM_3D, sides+2, 0, vertices);
    xGuLoadStates();
}

inline void xDrawPolygon(float radius, u16 sides)
{
    xDrawStar(radius, sides, 1);
}

inline void xDrawCircle(float radius, u16 verts_per_quadrant)
{
    if (verts_per_quadrant <= 0) return;
    xDrawPolygon(radius, 4*verts_per_quadrant + 1);
}

void xDrawBox(float width, float height)
{
    VertexF* vertices = (VertexF*)sceGuGetMemory(4*sizeof(VertexF));
    vertices[0].x = -0.5f*width;
    vertices[0].y =  0.5f*height;
    vertices[0].z =  0.0f;
    vertices[1].x = -0.5f*width;
    vertices[1].y = -0.5f*height;
    vertices[1].z =  0.0f;
    vertices[2].x =  0.5f*width;
    vertices[2].y = -0.5f*height;
    vertices[2].z =  0.0f;
    vertices[3].x =  0.5f*width;
    vertices[3].y =  0.5f*height;
    vertices[3].z =  0.0f;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_TRIANGLE_FAN, VertexF_vtype|GU_TRANSFORM_3D, 4, 0, vertices);
    xGuLoadStates();
}

void xDraw3DRect(ScePspFVector3* pos, float length, float height, u32 c)
{
    ScePspFMatrix4 view_mat;
    ScePspFVector3 translate;

    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();

    x_billboard(&translate, pos, &view_mat);

    sceGumLoadIdentity();
    sceGumTranslate(&translate);
    xGumScale(length, height, 1.0f);
    sceGuColor(c);

    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_TEXTURE_2D);

    xGumDrawUnitTexQuad();

    xGuLoadStates();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
}

void xDraw3DLine(ScePspFVector3* cam, ScePspFVector3* p1, ScePspFVector3* p2, float h1, float h2, u32 c)
{
    if ((cam->x == p1->x && cam->y == p1->y && cam->z == p1->z) ||
        (p1->x == p2->x && p1->y == p2->y && p1->z == p2->z)) return;
    ScePspFVector3 up;
    ScePspFVector3 dir = {p2->x - p1->x, p2->y - p1->y, p2->z - p1->z};

    x_billboard_dir(&up, cam, p1, &dir);
    x_normalize(&up, 0.5f);

    VertexF* vertices = (VertexF*)sceGuGetMemory(4*sizeof(VertexF));
    vertices[0].x = p1->x - h1*up.x;
    vertices[0].y = p1->y - h1*up.y;
    vertices[0].z = p1->z - h1*up.z;
    vertices[1].x = p1->x + h1*up.x;
    vertices[1].y = p1->y + h1*up.y;
    vertices[1].z = p1->z + h1*up.z;
    vertices[2].x = p2->x + h2*up.x;
    vertices[2].y = p2->y + h2*up.y;
    vertices[2].z = p2->z + h2*up.z;
    vertices[3].x = p2->x - h2*up.x;
    vertices[3].y = p2->y - h2*up.y;
    vertices[3].z = p2->z - h2*up.z;

    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuColor(c);
    sceGumDrawArray(GU_TRIANGLE_FAN, VertexF_vtype|GU_TRANSFORM_3D, 4, 0, vertices);
    xGuLoadStates();
}
