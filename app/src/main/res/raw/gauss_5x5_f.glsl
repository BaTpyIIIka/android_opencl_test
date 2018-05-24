precision mediump float;

uniform sampler2D sTexture;
uniform vec2 uPxD;
varying vec2 vTexCoord;


void main() {
    vec4 px00 = texture2D(sTexture, vTexCoord + vec2(-2.0*uPxD.x,-2.0*uPxD.y)) * 0.0030;
    vec4 px10 = texture2D(sTexture, vTexCoord + vec2(-1.0*uPxD.x,-2.0*uPxD.y)) * 0.0133;
    vec4 px20 = texture2D(sTexture, vTexCoord + vec2(0.0*uPxD.x,-2.0*uPxD.y)) * 0.0219;
    vec4 px30 = texture2D(sTexture, vTexCoord + vec2(1.0*uPxD.x,-2.0*uPxD.y)) * 0.0133;
    vec4 px40 = texture2D(sTexture, vTexCoord + vec2(2.0*uPxD.x,-2.0*uPxD.y)) * 0.0030;
    vec4 px01 = texture2D(sTexture, vTexCoord + vec2(-2.0*uPxD.x,-1.0*uPxD.y)) * 0.0133;
    vec4 px11 = texture2D(sTexture, vTexCoord + vec2(-1.0*uPxD.x,-1.0*uPxD.y)) * 0.0596;
    vec4 px21 = texture2D(sTexture, vTexCoord + vec2(0.0*uPxD.x,-1.0*uPxD.y)) * 0.0983;
    vec4 px31 = texture2D(sTexture, vTexCoord + vec2(1.0*uPxD.x,-1.0*uPxD.y)) * 0.0596;
    vec4 px41 = texture2D(sTexture, vTexCoord + vec2(2.0*uPxD.x,-1.0*uPxD.y)) * 0.0133;
    vec4 px02 = texture2D(sTexture, vTexCoord + vec2(-2.0*uPxD.x,0.0*uPxD.y)) * 0.0219;
    vec4 px12 = texture2D(sTexture, vTexCoord + vec2(-1.0*uPxD.x,0.0*uPxD.y)) * 0.0983;
    vec4 px22 = texture2D(sTexture, vTexCoord + vec2(0.0*uPxD.x,0.0*uPxD.y)) * 0.1621;
    vec4 px32 = texture2D(sTexture, vTexCoord + vec2(1.0*uPxD.x,0.0*uPxD.y)) * 0.0983;
    vec4 px42 = texture2D(sTexture, vTexCoord + vec2(2.0*uPxD.x,0.0*uPxD.y)) * 0.0219;
    vec4 px03 = texture2D(sTexture, vTexCoord + vec2(-2.0*uPxD.x,1.0*uPxD.y)) * 0.0133;
    vec4 px13 = texture2D(sTexture, vTexCoord + vec2(-1.0*uPxD.x,1.0*uPxD.y)) * 0.0596;
    vec4 px23 = texture2D(sTexture, vTexCoord + vec2(0.0*uPxD.x,1.0*uPxD.y)) * 0.0983;
    vec4 px33 = texture2D(sTexture, vTexCoord + vec2(1.0*uPxD.x,1.0*uPxD.y)) * 0.0596;
    vec4 px43 = texture2D(sTexture, vTexCoord + vec2(2.0*uPxD.x,1.0*uPxD.y)) * 0.0133;
    vec4 px04 = texture2D(sTexture, vTexCoord + vec2(-2.0*uPxD.x,2.0*uPxD.y)) * 0.0030;
    vec4 px14 = texture2D(sTexture, vTexCoord + vec2(-1.0*uPxD.x,2.0*uPxD.y)) * 0.0133;
    vec4 px24 = texture2D(sTexture, vTexCoord + vec2(0.0*uPxD.x,2.0*uPxD.y)) * 0.0219;
    vec4 px34 = texture2D(sTexture, vTexCoord + vec2(1.0*uPxD.x,2.0*uPxD.y)) * 0.0133;
    vec4 px44 = texture2D(sTexture, vTexCoord + vec2(2.0*uPxD.x,2.0*uPxD.y)) * 0.0030;

    vec4 clr = px00 + px10 + px20 + px30 + px40
                + px01 + px11 + px21 + px31 + px41
                + px02 + px12 + px22 + px32 + px42
                + px03 + px13 + px23 + px33 + px43
                + px04 + px14 + px24 + px34 + px44;

    gl_FragColor = clr;
}