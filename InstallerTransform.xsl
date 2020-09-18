<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:wix="http://schemas.microsoft.com/wix/2006/wi"
    xmlns="http://schemas.microsoft.com/wix/2006/wi" exclude-result-prefixes="wix">

    <xsl:template match="@*|*">
        <xsl:copy>
            <xsl:apply-templates select="@*" />
            <xsl:apply-templates select="*" />
        </xsl:copy>
    </xsl:template>

    <xsl:template match="wix:File[@Source[contains(.,'OpenRGB.exe')]]">
        <xsl:copy>
            <xsl:copy-of select="@*" />
            <xsl:attribute name="Id">
                <xsl:text>EXE</xsl:text>
            </xsl:attribute>
        </xsl:copy>
    </xsl:template>
</xsl:stylesheet>