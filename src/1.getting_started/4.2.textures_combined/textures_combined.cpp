#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <GLWrapperCore>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>

#include <iostream>

using namespace dream::glwrapper;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("4.2.texture.vs", "4.2.texture.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
	std::vector<float> vertices = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
	std::vector< int > indices= {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
	GLVertexArray vertexArray;
	GLArrayBuffer arrayBuffer;
	GLElementArrayBuffer elemArrayBuff;

	arrayBuffer.create(vertices, GL_STATIC_DRAW);
	elemArrayBuff.create(indices, GL_STATIC_DRAW);
	auto vertexSpec = GLVertexArray::AttributeSpecification {
		.components = 3,
		.location = 0,
		.normalize = GL_FALSE,
		.offset = 0,
		.stride = sizeof(float) * 8,
		.type = GL_FLOAT,
	};
	auto colorSpec = vertexSpec;
	colorSpec.location = 1;
	colorSpec.offset = 3 * sizeof(float);
	auto textureSpec = vertexSpec;
	textureSpec.location = 2;
	textureSpec.offset = 6 * sizeof(float);
	textureSpec.components = 2;
	vertexArray.createAttribute(colorSpec, arrayBuffer, elemArrayBuff);
	vertexArray.createAttribute(vertexSpec, arrayBuffer, elemArrayBuff);
	vertexArray.createAttribute(textureSpec, arrayBuffer, elemArrayBuff);


    // load and create a texture 
    // -------------------------
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (!data)
		throw std::runtime_error("Failed to load texture" + FileSystem::getPath("resources/textures/container.jpg"));
	std::vector<char> vecData(data, &data[width * height * nrChannels + 1]);
	GLTexture2D texture1;
	texture1.setParameter(GLTexture2D::ValueMagFilter::MagLinear);
	texture1.setParameter(GLTexture2D::ValueMinFilter::MinLinearMipmapLinear);
	texture1.setParameter(GLTexture2D::ValueWrapS::RepeatS);
	texture1.setParameter(GLTexture2D::ValueWrapT::RepeatT);
	texture1.create<char, 2>(vecData,
					0,
					GLTexture2D::InternalFormat::Rgb,
					GLTexture2D::DataFormat::DRgb,
					GLTexture2D::DataType::UByte,
					std::array<size_t, 2>{static_cast<GLuint>(width), static_cast<GLuint>(height)});
	texture1.generateMipMaps();
	stbi_image_free(data);
    // texture 2
    // ---------
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
	if (!data)
		throw std::runtime_error("Failed to load texture from file " + FileSystem::getPath("resources/textures/awesomeface.png"));
	vecData = std::vector<char>(data, &data[width * height * nrChannels]);
	GLTexture2D texture2;
	texture2.setParameter(GLTexture2D::ValueMagFilter::MagLinear);
	texture2.setParameter(GLTexture2D::ValueMinFilter::MinLinearMipmapLinear);
	texture2.setParameter(GLTexture2D::ValueWrapS::RepeatS);
	texture2.setParameter(GLTexture2D::ValueWrapT::RepeatT);
	texture2.create<char, 2>(vecData,
					0,
					GLTexture2D::InternalFormat::Rgba,
					GLTexture2D::DataFormat::DRgba,
					GLTexture2D::DataType::UByte,
					std::array<size_t, 2>{static_cast<size_t>(width), static_cast<size_t>(height)});
	texture2.generateMipMaps();
	stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture1);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, texture2);
		GLTextureUnit tu1(GL_TEXTURE0);
		GLTextureUnit tu2(GL_TEXTURE1);
		GLObjectBinder bindTu1(tu1);
		GLObjectBinder bintTx1(texture1);
		GLObjectBinder bindTu2(tu2);
		GLObjectBinder bintTx2(texture2);
		// render container
        ourShader.use();
		GLObjectBinder bintVAO(vertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
