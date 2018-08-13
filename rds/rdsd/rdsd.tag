<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="class">
    <name>std::ConfFile</name>
    <filename>classstd_1_1ConfFile.html</filename>
    <member kind="function">
      <type>int</type>
      <name>LoadConf</name>
      <anchor>a2</anchor>
      <arglist>(string filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>GetSectionCount</name>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ConfSection *</type>
      <name>GetSection</name>
      <anchor>a4</anchor>
      <arglist>(int index)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>std::ConfSection</name>
    <filename>classstd_1_1ConfSection.html</filename>
    <member kind="function">
      <type>string</type>
      <name>GetName</name>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetName</name>
      <anchor>a3</anchor>
      <arglist>(string newname)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>AddValue</name>
      <anchor>a4</anchor>
      <arglist>(string name, string value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>GetValueCount</name>
      <anchor>a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ConfValue *</type>
      <name>GetValue</name>
      <anchor>a6</anchor>
      <arglist>(int index)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>std::ConfValue</name>
    <filename>classstd_1_1ConfValue.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>IsSet</name>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>string</type>
      <name>GetName</name>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetName</name>
      <anchor>a4</anchor>
      <arglist>(string newname)</arglist>
    </member>
    <member kind="function">
      <type>string</type>
      <name>GetString</name>
      <anchor>a5</anchor>
      <arglist>(bool &amp;valid)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>GetInt</name>
      <anchor>a6</anchor>
      <arglist>(bool &amp;valid)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>GetDouble</name>
      <anchor>a7</anchor>
      <arglist>(bool &amp;valid)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetString</name>
      <anchor>a8</anchor>
      <arglist>(string newvalue)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetInt</name>
      <anchor>a9</anchor>
      <arglist>(int newvalue)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetDouble</name>
      <anchor>a10</anchor>
      <arglist>(double newvalue)</arglist>
    </member>
  </compound>
</tagfile>
