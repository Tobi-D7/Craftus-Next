/*  ____            __ _                   _   _           _   
*  / ___|_ __ __ _ / _| |_ _   _ ___      | \ | | _____  _| |_ 
* | |   | '__/ _` | |_| __| | | / __|_____|  \| |/ _ \ \/ / __|
* | |___| | | (_| |  _| |_| |_| \__ \_____| |\  |  __/>  <| |_ 
*  \____|_|  \__,_|_|  \__|\__,_|___/     |_| \_|\___/_/\_\\__|
*                                                              
*  _   _ ____ ___      ____ _____ ______ _____             
* | \ | |  _ \_ _|    |  _ \___  / /  _ \___  |_   ____  __
* |  \| | |_) | |_____| | | | / / /| | | | / /\ \ / /\ \/ /
* | |\  |  __/| |_____| |_| |/ / / | |_| |/ /  \ V /  >  < 
* |_| \_|_|  |___|    |____//_/_/  |____//_/    \_/  /_/\_\
* Copyright (C) 2022-2023 Tobi-D7, RSDuck, Onixiya, D7vx-Dev, NPI-D7
*/
#pragma once
#include <objects/Object.hpp>
#include <rendering/Renderer_Def.hpp>
#include <rendering/Vertex.hpp>
#include <ShaderTable.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Base {
class Sprite : public Base::Object {
public:
  Sprite() {
    Base::UiSquare vtx[] = {
        {{0.0f, 1.0f}, {0.0f, 1.0f}, {0, 0, 0, 0}},
        {{1.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, 0, 0}},
        {{0.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, 0, 0}},

        {{0.0f, 1.0f}, {0.0f, 1.0f}, {0, 0, 0, 0}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}, {0, 0, 0, 0}},
        {{1.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, 0, 0}},
    };
    trishader = new BaseShader();
    auto s_sp_shader_ = Base::GetShader(SHADER_SPRITE);
    trishader->LoadCode(s_sp_shader_.first.c_str(), s_sp_shader_.second.c_str());
    trishader->use();

    vao_ = new BaseVertexArray();
    vao_->Create(&vtx, NPI_ARS(vtx), sizeof(Base::UiSquare));
    vao_->AddAttrInfo(0, 2, 0, false, sizeof(Base::UiSquare),
                      (void *)offsetof(Base::UiSquare, position));
    vao_->AddAttrInfo(1, 2, 0, false, sizeof(Base::UiSquare),
                      (void *)offsetof(Base::UiSquare, texcoords));
    vao_->AddAttrInfo(2, 4, 0, false, sizeof(Base::UiSquare),
                      (void *)offsetof(Base::UiSquare, color));
    vao_->UnBind();
  }
  void Draw(bvec2i raster_box) override {
    trishader->use();
    glm::mat4 projection = glm::ortho(0.0f, (float)raster_box.x,
                                      (float)raster_box.y, 0.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
                           glm::vec3(glm::vec2(position.x, position.y), 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * texture->GetSize().x,
                                            0.5f * texture->GetSize().y, 0.0f));
    model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
    model =
        glm::translate(model, glm::vec3(-0.5f * texture->GetSize().x,
                                        -0.5f * texture->GetSize().y, 0.0f));

    model = glm::scale(
        model, glm::vec3(texture->GetSize().x, texture->GetSize().y, 1.0f));

    this->trishader->setMat4("projection", projection);
    this->trishader->setMat4("model", model);
    this->trishader->setVec3("spriteColor", glm::vec3(1.0f));
    texture->Bind();
    vao_->Bind();
    Base_drawArrays(0, 6);
    vao_->UnBind();
  }

  void SetTexture(BaseTexture &tex) {
    texture = new BaseTexture;
    texture = &tex;
  }

  BaseTexture *GetTexture() { return texture; }

  void SetPosition(bvec2f t_pos) { position = t_pos; }
  bvec2f GetPosition() { return position; }
  void SetScale(bvec2f t_scale) { scale = t_scale; }
  bvec2f GetScale() { return scale; }
  void SetRotation(float rotation) { rot = rotation; }
  float GetRotation() { return rot; }

private:
  BaseVertexArray *vao_;
  BaseShader *trishader;
  BaseTexture *texture;
  bvec2f position = bvec2f(0, 0);
  bvec2f scale = bvec2f(1, 1);
  float rot = 0;
};
} // namespace Base