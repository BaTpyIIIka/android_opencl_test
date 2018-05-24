package ru.michaelilyin.alg1;

import org.junit.Test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {

    @Test
    public void addition_isCorrect() throws Exception {
        String[] files = new String[] {
            "/home/michael/test/text1.txt",
            "/home/michael/test/text2.txt",
            "/home/michael/test/text3.txt",
            "/home/michael/test/text4.txt",
            "/home/michael/test/text5.txt"
        };
        Pattern rsPush = Pattern.compile("rs push:\\s(.*)");
        Pattern rsExec = Pattern.compile("rs exec:\\s(.*)");
        Pattern rsPull = Pattern.compile("rs pull:\\s(.*)");

        Pattern fsPush = Pattern.compile("fs push:\\s(.*)");
        Pattern fsExec = Pattern.compile("fs exec:\\s(.*)");
        Pattern fsPull = Pattern.compile("fs pull:\\s(.*)");

        Pattern oclPush = Pattern.compile("ocl push:\\s(.*)");
        Pattern oclExec = Pattern.compile("ocl exec:\\s(.*)");
        Pattern oclPull = Pattern.compile("ocl pull:\\s(.*)");
        
        for (String file : files) {
            ArrayList<String> rsPushData = new ArrayList<>();
            ArrayList<String> rsExecData = new ArrayList<>();
            ArrayList<String> rsPullData = new ArrayList<>();

            ArrayList<String> fsPushData = new ArrayList<>();
            ArrayList<String> fsExecData = new ArrayList<>();
            ArrayList<String> fsPullData = new ArrayList<>();

            ArrayList<String> oclPushData = new ArrayList<>();
            ArrayList<String> oclExecData = new ArrayList<>();
            ArrayList<String> oclPullData = new ArrayList<>();
            try(BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    //region while body
                    Matcher rsPushMatcher = rsPush.matcher(line);
                    if (rsPushMatcher.find()) {
                        rsPushData.add(rsPushMatcher.group(1));
                    }

                    Matcher rsExecMatcher = rsExec.matcher(line);
                    if (rsExecMatcher.find()) {
                        rsExecData.add(rsExecMatcher.group(1));
                    }

                    Matcher rsPullMatcher = rsPull.matcher(line);
                    if (rsPullMatcher.find()) {
                        rsPullData.add(rsPullMatcher.group(1));
                    }

                    Matcher fsPushMatcher = fsPush.matcher(line);
                    if (fsPushMatcher.find()) {
                        fsPushData.add(fsPushMatcher.group(1));
                    }

                    Matcher fsExecMatcher = fsExec.matcher(line);
                    if (fsExecMatcher.find()) {
                        fsExecData.add(fsExecMatcher.group(1));
                    }

                    Matcher fsPullMatcher = fsPull.matcher(line);
                    if (fsPullMatcher.find()) {
                        fsPullData.add(fsPullMatcher.group(1));
                    }

                    Matcher oclPushMatcher = oclPush.matcher(line);
                    if (oclPushMatcher.find()) {
                        oclPushData.add(oclPushMatcher.group(1));
                    }

                    Matcher oclExecMatcher = oclExec.matcher(line);
                    if (oclExecMatcher.find()) {
                        oclExecData.add(oclExecMatcher.group(1));
                    }

                    Matcher oclPullMatcher = oclPull.matcher(line);
                    if (oclPullMatcher.find()) {
                        oclPullData.add(oclPullMatcher.group(1));
                    }
                    //endregion
                }
            }
            System.out.println("Push -------------------------");
            for (String num : rsPushData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : fsPushData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : oclPushData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            System.out.println("Exec -------------------------");
            for (String num : rsExecData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : fsExecData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : oclExecData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            System.out.println("Pull -------------------------");
            for (String num : rsPullData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : fsPullData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            for (String num : oclPullData) {
                System.out.printf("%s ", num);
            }
            System.out.println();
            System.out.println("=======================");
        }
    }
}