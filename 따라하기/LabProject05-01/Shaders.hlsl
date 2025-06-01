
//정점 쉐이더를 정의한다. 렌더 타겟 사각형 전체를 삼각형 2개로 표현한다
float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output = (float4) 0;
    if (nVertexID == 0)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 1)
        output = float4(+1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 2)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 3)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 4)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 5)
        output = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    return (output);
}

#define FRAME_BUFFER_WIDTH 640.0f
#define FRAME_BUFFER_HEIGHT 480.0f
//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. -> 가로 그라데이션
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    cColor.r = input.x / FRAME_BUFFER_WIDTH;
//    return(cColor);
//}

//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. -> 역시.. 그라데이션.
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    cColor.r = input.x / FRAME_BUFFER_WIDTH;
//    cColor.g = input.y / FRAME_BUFFER_HEIGHT;
//    return (cColor);
//}

//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET // 흑백 그라데이션인데 왜 난 가운데가 제대로 안맞는지 모르겠넹
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    cColor.rgb = distance(float2(0.0f, 0.0f), input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
//    cColor.rgb = length(input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
//    cColor.rgb = distance(float2(0.0f, 0.0f), (input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT) - 0.5f)); // (0, 1) : (-0.5, 0.5)
//    cColor.rgb = length(input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT) - 0.5f);
//    cColor.rgb = distance(float2(0.5f, 0.5f), input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
//    return (cColor);
//}

//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. -> 파란색 타원... 인데 여전히 가운데가 안맞아 : 화면크기때문임 고쳤음
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    float fDistance = distance(float2(0.5f, 0.5f), input.xy / float2(FRAME_BUFFER_WIDTH,
//    FRAME_BUFFER_HEIGHT));
//    if (fDistance < 0.25f)
//        cColor.b = 1.0f;
//    return (cColor);
//}

//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET // 파란색 원
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f; // (0, 1) : (-0.5, 0.5)
//    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
//    // cColor.b = step(length(f2NDC), 0.25f); //step(x, y) = (x <= y) ? 1 : 0
//    cColor.b = (length(f2NDC) <= 0.25f) ? 1.0f : 0.0f; //step(x, y) = (x <= y) ? 1 : 0
//    return (cColor);
//}

//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f;
//    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
//    float fLength = length(f2NDC);
//    float fMin = 0.3f, fMax = 0.2f;
//    cColor.rgb = smoothstep(fMin, fMax, fLength);
//    return (cColor);
//}

// 여긴 그리드에 파란 원
//#define HALF_WIDTH (FRAME_BUFFER_WIDTH * 0.5f)
//#define HALF_HEIGHT (FRAME_BUFFER_HEIGHT * 0.5f)
//#define EPSILON 1.0e-5f
//inline bool IsZero(float fValue)
//{
//    return ((abs(fValue) <= EPSILON));
//}
//inline bool IsZero(float fValue, float fEpsilon)
//{
//    return ((abs(fValue) <= fEpsilon));
//}
//inline bool IsEqual(float fA, float fB, float fEpsilon)
//{
//    return ((abs(fA - fB) <=
//fEpsilon));
//}
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    if ((int) input.x == (int) HALF_WIDTH)
//        cColor.g = 1.0f;
//    if ((int) input.y == (int) HALF_HEIGHT)
//        cColor.r = 1.0f;
//    float fDistance = distance((int2) input.xy, float2(HALF_WIDTH, HALF_HEIGHT));
//    if (IsEqual(fDistance, 100.0f, 0.5f))
//        cColor.b = 1.0f;
//    return (cColor);
//}

//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. -> 파란 네모를 그리다니..
//float Rectangle(float2 f2NDC, float fLeft, float fRight, float fTop, float fBottom)
//{
//    float2 f2Shape = float2(step(fLeft, f2NDC.x), step(f2NDC.x, fRight));
//    f2Shape *= float2(step(fTop, f2NDC.y), step(f2NDC.y, fBottom));
//    return(f2Shape.x * f2Shape.y);
//}
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f; // (0, 1) : (-0.5, 0.5)
//    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
//    float2 f2Horizontal = float2(0.1f, +0.3f); //(Left, Right)
//    float2 f2Vertical = float2(-0.3f, +0.3f); //(Top, Bottom)
//    cColor.b = Rectangle(f2NDC, +0.1f, +0.3f, -0.2f, +0.4f);
//    cColor.b += Rectangle(f2NDC, -0.3f, -0.1f, -0.4f, -0.1f);
//    return (cColor);
//}

//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. -> 뭔가 징그러운 모양이군
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    float2 f2NDC = float2(input.x - FRAME_BUFFER_WIDTH * 0.5f, input.y -
//    FRAME_BUFFER_HEIGHT * 0.5f);
//    f2NDC *= 20.0f;
//    float fLength = length(f2NDC);
//    cColor.rgb = cos(fLength);
//    return(cColor);
//}

// 중략하자..